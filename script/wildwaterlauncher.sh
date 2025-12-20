#!/bin/bash

# ===========================================
# WildWater All-In-One Launcher (v2.1)
# ===========================================

# Color Definitions
teal="\033[38;5;116m"
blue="\033[38;5;153m"
mauve="\033[38;5;146m"
yellow="\033[38;5;222m"
grey="\033[38;5;250m"
bg_dark="\033[48;5;116m"
bg_dark2="\033[48;5;146m"
fg_dark="\033[38;5;234m"
reset="\033[0m"

# Utility to get visual length (strips ANSI codes)
get_len() {
    echo -ne "$1" | sed 's/\x1b\[[0-9;]*m//g' | wc -m
}

show_help() {
    # 1. Prepare the lines as variables
    local l_title=" ${teal}◖${bg_dark}${fg_dark} WildWater Shell Launcher ${reset}${teal}◗${reset} "
    local l_usage="  ${blue}Usage:${reset} $0 DATAFILE COMMAND [PARAMS]"
    local l_opt_h=" ${mauve}◖${bg_dark2}${fg_dark} OPTIONS ${reset}${mauve}◗${reset}"
    local l_opt1="    ${teal}--debug${reset}      Verbose trace"
    local l_opt2="    ${teal}--fast${reset}       Max speed"
    local l_opt3="    ${teal}--rsc${reset}        Reset cache"
    local l_opt3="    ${teal}--make${reset}       Compile program"
    local l_cmd_h=" ${mauve}◖${bg_dark2}${fg_dark} COMMANDS ${reset}${mauve}◗${reset}"
    local l_cmd1="    ${blue}histo TYPE${reset}   Generates histogram"
    local l_sub1="    ${grey}- options: max, vol   ${reset}"
    local l_cmd2="    ${blue}leaks ID${reset}     Computes leaks"
	
    # 2. Find max width automatically
    local max_v=0
    for l in "$l_title" "$l_usage" "$l_opt1" "$l_opt2" "$l_opt3" "$l_cmd1" "$l_sub1" "$l_cmd2"; do
        local v=$(get_len "$l")
        (( v > max_v )) && max_v=$v
    done
	
    local inner_w=$(( max_v + 2 ))
    local term_w=$(tput cols 2>/dev/null || echo 80)
    (( inner_w > term_w - 5 )) && inner_w=$(( term_w - 5 ))
	
    # Helper: Print Row
    print_row() {
        local b_col=$1; local content=$2
        local v_len=$(get_len "$content")
        local pad=$(( inner_w - v_len ))
        echo -ne " ${b_col}│${reset}${content}"
        printf '%*s' "$pad" ""
        echo -e "${b_col}│${reset}"
    }

    # Helper: Draw Border
    draw_hr() {
        local col=$1; local l=$2; local m=$3; local r=$4
        echo -ne " ${col}${l}"
        for ((i=0; i<inner_w; i++)); do echo -n "$m"; done
        echo -e "${r}${reset}"
    }

    echo ""
    draw_hr "$blue" "╭" "─" "╮"

    local t_v=$(get_len "$l_title")
    local t_pad=$(( (inner_w - t_v) / 2 ))
    local t_res=$(( inner_w - t_v - t_pad ))
    echo -ne " ${blue}│${reset}$(printf '%*s' "$t_pad" "")${l_title}$(printf '%*s' "$t_res" "")"
    echo -e "${blue}│${reset}"

    print_row "$teal" "$l_usage"
    draw_hr "$teal" "├" "─" "┤"
    print_row "$teal" ""
    print_row "$teal" "$l_opt_h"
    print_row "$teal" "$l_opt1"
    print_row "$teal" "$l_opt2"
    print_row "$teal" "$l_opt3"
    print_row "$teal" ""
    print_row "$teal" "$l_cmd_h"
    print_row "$teal" "$l_cmd1"
    print_row "$teal" "$l_sub1"
    print_row "$teal" "$l_cmd2"
    draw_hr "$teal" "╰" "─" "╯"
    echo ""
    exit 1
}

# 1. Parameter Parsing
DEBUG=false; FAST=false; RESET_CACHE=false; RAW_ARGS=()
for arg in "$@"; do
    case $arg in
        --debug) DEBUG=true ;;
        --fast)  FAST=true ;;
        --rsc)   RESET_CACHE=true ;;
        *)       RAW_ARGS+=("$arg") ;;
    esac
done

if [ "${#RAW_ARGS[@]}" -lt 2 ]; then show_help; fi

DATAFILE="${RAW_ARGS[0]}"
COMMAND="${RAW_ARGS[1]}"
PARAM="${RAW_ARGS[2]}"

# [ADDED]
EFFECTIVE_DATAFILE="$DATAFILE"
FILTERED=false

log_debug() { [[ "$DEBUG" == true ]] && echo -e "${grey}[DEBUG]${reset} $1"; }

# 2. Preparation
if [ ! -f "$DATAFILE" ]; then echo -e "${red}Error: '$DATAFILE' not found.${reset}"; exit 1; fi

CACHE_DIR="/home/$USER/.cache/wildwater_cache"
TEMP_DIR="/dev/shm/wildwater_$$"
mkdir -p "$TEMP_DIR"
LFN="$(cat "$CACHE_DIR/lfn")"
USE_CACHE=false

# [ADDED]
if [ "$COMMAND" = "leaks" ]; then
    log_debug "Leaks mode: filtering input file"
    FILTERED=true
    REDUCED_FILE="$TEMP_DIR/reduced_input.dat"
    touch REDUCED_FILE
    LC_ALL=C grep -F "$PARAM" "$DATAFILE" > "$REDUCED_FILE"
    EFFECTIVE_DATAFILE="$REDUCED_FILE"
fi

if [ -d "$CACHE_DIR" ] && \
   [ -s "$CACHE_DIR/plants.dat" ] && \
   [ "$CACHE_DIR/plants.dat" -nt "$DATAFILE" ] && \
   [ "$LFN" == "$DATAFILE" ] && \
   [ "$RESET_CACHE" != true ] && \
   [ "$FILTERED" != true ]; then
    log_debug "Cache hit. Moving to RAM..."
    cp "$CACHE_DIR"/*.dat "$TEMP_DIR/"
    USE_CACHE=true
fi

start=$(date +%s)

# 3. Optimized Sorting with Optional Progress Bar
if [ "$USE_CACHE" = false ]; then

    # [ADDED]
    TOTAL_LINES=$(wc -l < "$EFFECTIVE_DATAFILE")

    log_debug "Sorting $TOTAL_LINES lines..."

    LC_ALL=C awk -F';' -v tmp="$TEMP_DIR" -v tot="$TOTAL_LINES" -v fast="$FAST" '
        BEGIN {
            f1=tmp"/plants.dat"; f2=tmp"/sources_to_plants.dat"; f3=tmp"/plants_to_storage.dat"
            f4=tmp"/storage_to_junction.dat"; f5=tmp"/junction_to_service.dat"; f6=tmp"/service_to_customer.dat"
        }
        {
            if (fast == "false" && NR % 20000 == 0) {
                printf "\r \033[38;5;116mSorting: [%-20s] %d%%\033[0m", \
                substr("####################", 1, int(NR/tot*20)), int(NR/tot*100) > "/dev/stderr"
            }
            if ($1 == "-") {
                if ($5 == "-") {
                    if ($3 == "-") { print > f1 } else if ($4 == "-") { print > f3 }
                } else if ($4 != "-") { print > f2 }
            } else if ($4 == "-") {
                if (index($3, "Cust #")) { print > f6 }
                else if (index($3, "Service #")) { print > f5 }
                else if (index($3, "Junction #")) { print > f4 }
            }
        }
        END { if (fast == "false") printf "\r \033[32mSorting Complete! [####################] 100%%\033[0m\n" > "/dev/stderr" }
    ' "$EFFECTIVE_DATAFILE"
fi

if [ "$FILTERED" = true ]; then
	log_debug "Temporary reduce file has been handled removing : $EFFECTIVE_DATAFILE"
	rm -f "$EFFECTIVE_DATAFILE"
fi

echo "idk"
# 4. Binary Execution
WILDCARD_EXEC="$(cd "$(dirname "$0")/.." && pwd)/linkingpark"
echo "idk"

case "$COMMAND" in
    histo)
        [[ -z "$PARAM" ]] && exit 1
        CSV_FILE="${PARAM}_histogram_$(date +%Y%m%d_%H%M%S).csv"
        #touch "$CSV_FILE"
		if [ "$PARAM" = "max" ] || [ "$PARAM" = "vol" ]; then
        	log_debug "Executing binary: histo_data"
        	"$WILDCARD_EXEC" "$TEMP_DIR" "histo" "$PARAM" > "$CSV_FILE"
		fi
        echo -e "${teal}Output:${reset} $CSV_FILE"
        ;;
    leaks)
        [[ -z "$PARAM" ]] && exit 1
        echo uwu
        "$WILDCARD_EXEC" "$TEMP_DIR" leaks "$PARAM"
        echo uwu
        ;;
esac

# 5. Finalize
if [ "$USE_CACHE" = false ] && [ "$FILTERED" = false ]; then
    mkdir -p "$CACHE_DIR"
    cp "$TEMP_DIR"/*.dat "$CACHE_DIR/"
    echo "$DATAFILE" > "$CACHE_DIR/lfn"
fi

rm -rf "$TEMP_DIR"
echo -e "${blue}Total Time : $(( $(date +%s) - start ))s${reset}"
