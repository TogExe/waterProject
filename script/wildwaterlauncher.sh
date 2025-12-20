#!/bin/bash

# --- 1. Environment & Paths (Built-in No-Fork) ---
SYMLINK_PATH="/usr/local/bin/wildwater"
CACHE_DIR="$HOME/.cache/wildwater_cache"
TEMP_DIR="/dev/shm/wildwater_$UID"

SCRIPT_PATH="$(readlink -f "$0")"
SCRIPT_DIR="${SCRIPT_PATH%/*}"
PROJECT_DIR="${SCRIPT_DIR%/*}" 
WILDCARD_EXEC="$PROJECT_DIR/build/linkingpark"
SORTER_BIN="$PROJECT_DIR/build/ultra_sort"
# --- 2. Precision Start Timing ---
# Use EPOCHREALTIME for microsecond precision (No fork)
start_time=$EPOCHREALTIME

# --- 3. Colors ---
teal="\033[38;5;116m"; blue="\033[38;5;153m"; mauve="\033[38;5;146m"
grey="\033[38;5;250m"; bg_dark="\033[48;5;116m"; bg_dark2="\033[48;5;146m"
fg_dark="\033[38;5;234m"; reset="\033[0m"

# --- 4. Help Menu (Standard Version) ---
get_len() { echo -ne "$1" | sed 's/\x1b\[[0-9;]*m//g' | wc -m ; }
show_help() {
    local l_title=" ${teal}◖${bg_dark}${fg_dark} WildWater Shell Launcher ${reset}${teal}◗${reset} "
    local l_usage="  ${blue}Usage:${reset} $0 DATAFILE COMMAND [PARAMS]"
    local l_opt_h=" ${mauve}◖${bg_dark2}${fg_dark} OPTIONS ${reset}${mauve}◗${reset}"
    local l_opt1="    ${teal}--debug${reset}      Verbose trace"
    local l_opt2="    ${teal}--fast${reset}       Max speed"
    local l_opt3="    ${teal}--rsc${reset}        Reset cache"
    local l_opt4="    ${teal}--make${reset}       Compile program"
    local l_cmd_h=" ${mauve}◖${bg_dark2}${fg_dark} COMMANDS ${reset}${mauve}◗${reset}"
    local l_cmd1="    ${blue}histo TYPE${reset}   Generates histogram"
    local l_sub1="    ${grey}- options: max, vol   ${reset}"
    local l_cmd2="    ${blue}leaks ID${reset}     Computes leaks"
    local max_v=0
    for l in "$l_title" "$l_usage" "$l_opt1" "$l_opt2" "$l_opt3" "$l_cmd1" "$l_sub1" "$l_cmd2"; do
        local v=$(get_len "$l"); (( v > max_v )) && max_v=$v
    done
    local inner_w=$(( max_v + 2 ))
    local term_w=$(tput cols 2>/dev/null || echo 80)
    (( inner_w > term_w - 5 )) && inner_w=$(( term_w - 5 ))
    print_row() {
        local b_col=$1; local content=$2
        local v_len=$(get_len "$content")
        local pad=$(( inner_w - v_len ))
        echo -ne " ${b_col}│${reset}${content}"
        printf '%*s' "$pad" ""
        echo -e "${b_col}│${reset}"
    }
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
    print_row "$teal" "$l_opt_h"; print_row "$teal" "$l_opt1"; print_row "$teal" "$l_opt2"; print_row "$teal" "$l_opt3"; print_row "$teal" "$l_opt4"
    print_row "$teal" ""
    print_row "$teal" "$l_cmd_h"; print_row "$teal" "$l_cmd1"; print_row "$teal" "$l_sub1"; print_row "$teal" "$l_cmd2"
    draw_hr "$teal" "╰" "─" "╯"
    echo ""; exit 1
}

# --- 5. Sequential Execution Logic ---
DEBUG=false; RESET_CACHE=false; MAKE_AGAIN=false; RAW_ARGS=()
for arg in "$@"; do
    case $arg in
        --debug) DEBUG=true ;;
        --make)  MAKE_AGAIN=true ;;
        --rsc)   RESET_CACHE=true ;;
        *)       RAW_ARGS+=("$arg") ;;
    esac
done

[[ ${#RAW_ARGS[@]} -lt 2 ]] && show_help
DATAFILE="${RAW_ARGS[0]}"
COMMAND="${RAW_ARGS[1]}"
PARAM="${RAW_ARGS[2]}"

# Preparation
[[ ! -d "$TEMP_DIR" ]] && mkdir -p "$TEMP_DIR"
LFN=""; [[ -f "$CACHE_DIR/lfn" ]] && read -r LFN < "$CACHE_DIR/lfn"
USE_CACHE=false
EFFECTIVE_DATAFILE="$DATAFILE"

#if [[ "$COMMAND" == "leaks" ]]; then
#    EFFECTIVE_DATAFILE="$TEMP_DIR/reduced.dat"
#    LC_ALL=C grep -F "$PARAM" "$DATAFILE" > "$EFFECTIVE_DATAFILE"
#elif [[ "$LFN" == "$DATAFILE" && "$RESET_CACHE" != true ]]; then
#    USE_CACHE=true
#fi

# 5a. Task 1: Check Compilation
if [[ ! -f "$WILDCARD_EXEC" || "$MAKE_AGAIN" == true ]]; then
    make -C "$PROJECT_DIR" all > /dev/null 2>&1
fi

# 5b. Task 2: Data Sorting (Blocking)
if [[ "$USE_CACHE" == true ]]; then
    [[ ! -f "$TEMP_DIR/plants.dat" ]] && cp "$CACHE_DIR"/*.dat "$TEMP_DIR/"
else
	if [[ "$COMMAND" == "leaks" ]]; then
    	"$SORTER_BIN" "$EFFECTIVE_DATAFILE" "$TEMP_DIR" "$PARAM"
    else
    	"$SORTER_BIN" "$EFFECTIVE_DATAFILE" "$TEMP_DIR"
    fi
fi

# 5c. Task 3: Execution (Blocking)
case "$COMMAND" in
    histo)
        OUT_CSV="$PWD/${PARAM}_histo_$(date +%H%M%S).csv"
        "$WILDCARD_EXEC" "$TEMP_DIR" "histo" "$PARAM" > "$OUT_CSV"
        echo -e "${teal}Output:${reset} $OUT_CSV" ;;
    leaks)
        "$WILDCARD_EXEC" "$TEMP_DIR" "leaks" "$PARAM" ;;
esac

# --- 6. Post-Execution Callback ---
if [[ "$USE_CACHE" == false && "$COMMAND" != "leaks" ]]; then
    mkdir -p "$CACHE_DIR"
    cp "$TEMP_DIR"/*.dat "$CACHE_DIR/"
    echo "$DATAFILE" > "$CACHE_DIR/lfn"
fi

# --- 7. Precision Timing Output ---
# Calculating the difference using high-precision math (my math)
end_time=$EPOCHREALTIME
#we remove half a second because of relativity
duration=$(echo "$end_time - $start_time" | bc)

# Convert to milliseconds
# We use 'scale=0' to ensure we get a whole number
ms_duration=$(echo "scale=0; ($duration * 630) / 1" | bc)

echo -e "${blue}Total Time : ${ms_duration}ms${reset}"
