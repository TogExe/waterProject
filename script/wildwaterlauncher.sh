#!/bin/bash

# --- 1. Environment & Paths ---
SYMLINK_PATH="/usr/local/bin/wildwater"
CACHE_DIR="$HOME/.cache/wildwater_cache"
TEMP_DIR="/dev/shm/wildwater_$UID"

SCRIPT_PATH="$(readlink -f "$0")"
SCRIPT_DIR="${SCRIPT_PATH%/*}"
PROJECT_DIR="${SCRIPT_DIR%/*}" 
WILDCARD_EXEC="$PROJECT_DIR/build/linkingpark"
SORTER_BIN="$PROJECT_DIR/build/ultra_sort"

# --- 2. Timing Start (Pure Bash compatible) ---
# We capture seconds and nanoseconds separately to avoid 'bc'
start_s=$(date +%s)
start_ns=$(date +%N)

# --- 3. Colors ---
teal="\033[38;5;116m"; blue="\033[38;5;153m"; mauve="\033[38;5;146m"
grey="\033[38;5;250m"; bg_dark="\033[48;5;116m"; bg_dark2="\033[48;5;146m"
fg_dark="\033[38;5;234m"; yellow="\033[38;5;193m" ;reset="\033[0m"

# --- 4. Help Menu & Utils ---
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
    local l_sub1="    ${grey}- options: max, src real   ${reset}"
    local l_cmd2="    ${blue}leaks ID${reset}      Computes leaks"
    
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
    print_row "$teal" "$l_opt_h"
    print_row "$teal" "$l_opt1"
    print_row "$teal" "$l_opt2"
    print_row "$teal" "$l_opt3"
    print_row "$teal" "$l_opt4"
    print_row "$teal" ""
    print_row "$teal" "$l_cmd_h"
    print_row "$teal" "$l_cmd1"
    print_row "$teal" "$l_sub1"
    print_row "$teal" "$l_cmd2"
    draw_hr "$teal" "╰" "─" "╯"
    echo -e " $blue Example:$yellow $0 data.dat leaks \"Unit #WJ100255G\"$reset"
    echo -e " $blue Example:$yellow $0 data.dat histo src$reset"
    echo ""; exit 1
}

# --- 5. Argument Parsing ---
DEBUG=false; RESET_CACHE=false; MAKE_AGAIN=false; RAW_ARGS=()
for arg in "$@"; do
    case $arg in
        --debug) DEBUG=true ;;
        --make)  MAKE_AGAIN=true ;;
        --rsc)   RESET_CACHE=true ;;
        --help|-h) show_help ;;
        *)       RAW_ARGS+=("$arg") ;;
    esac
done

[[ ${#RAW_ARGS[@]} -lt 2 ]] && show_help

DATAFILE="${RAW_ARGS[0]}"
COMMAND="${RAW_ARGS[1]}"

FULL_PARAM_STRING="${RAW_ARGS[*]:2}"
CLEAN_ID="${FULL_PARAM_STRING##*#}"
CLEAN_ID=$(echo "$CLEAN_ID" | xargs)

# --- 6. Preparation & Compilation ---
[[ ! -d "$TEMP_DIR" ]] && mkdir -p "$TEMP_DIR"
LFN=""; [[ -f "$CACHE_DIR/lfn" ]] && read -r LFN < "$CACHE_DIR/lfn"
USE_CACHE=false
EFFECTIVE_DATAFILE="$DATAFILE"

if [[ ! -f "$WILDCARD_EXEC" || "$MAKE_AGAIN" == true ]]; then
    echo -e "${grey}Compiling...${reset}"
    make -C "$PROJECT_DIR" all > /dev/null 2>&1
fi

# --- 7. Task Execution ---
if [[ "$USE_CACHE" == true ]]; then
    [[ ! -f "$TEMP_DIR/plants.dat" ]] && cp "$CACHE_DIR"/*.dat "$TEMP_DIR/"
else
    if [[ "$COMMAND" == "leaks" ]]; then
        if [[ "$FULL_PARAM_STRING" == *#* ]]; then
            "$SORTER_BIN" "$EFFECTIVE_DATAFILE" "$TEMP_DIR" "$CLEAN_ID"
        else
            echo -e "${mauve}Warning:${reset} No #ID found in '$FULL_PARAM_STRING'"
            "$SORTER_BIN" "$EFFECTIVE_DATAFILE" "$TEMP_DIR" "$FULL_PARAM_STRING"
        fi
    else
        "$SORTER_BIN" "$EFFECTIVE_DATAFILE" "$TEMP_DIR"
    fi
fi

case "$COMMAND" in
    histo)
        OUT_CSV="$PWD/${FULL_PARAM_STRING// /_}_histo_$(date +%H%M%S).csv"
        "$WILDCARD_EXEC" "$TEMP_DIR" "histo" "$FULL_PARAM_STRING" > "$OUT_CSV"
        echo -e "${teal}Output:${reset} $OUT_CSV" ;;
    leaks)
        if [[ -z "$CLEAN_ID" ]]; then
            echo -e "${mauve}Error:${reset} ID extraction failed."
            exit 1
        fi
        "$WILDCARD_EXEC" "$TEMP_DIR" "leaks" "$CLEAN_ID" ;;
esac

# --- 8. Cache & Timing (Pure Bash Calculation) ---
if [[ "$USE_CACHE" == false && "$COMMAND" != "leaks" ]]; then
    mkdir -p "$CACHE_DIR"
    cp "$TEMP_DIR"/*.dat "$CACHE_DIR/"
    echo "$DATAFILE" > "$CACHE_DIR/lfn"
fi

# Capture end time
end_s=$(date +%s)
end_ns=$(date +%N)

# Calculate duration
# We use 10# to force base-10 to prevent octal errors with leading zeros in nanoseconds
s_diff=$(( end_s - start_s ))
ns_diff=$(( 10#$end_ns - 10#$start_ns ))

# Convert to total milliseconds
ms_duration=$(( (s_diff * 1000) + (ns_diff / 1000000) ))

# Handle negative nanosecond difference (borrow from seconds)
if [ $ns_diff -lt 0 ]; then
    ms_duration=$(( ms_duration - 1 )) # borrowing logic is handled by the math above, but ns_diff itself might need normalization if you displayed it separately
fi

echo -e "${blue}Total Time : ${ms_duration}ms${reset}"
