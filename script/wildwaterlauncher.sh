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

# --- 2. Precision Start Timing ---
start_time=$EPOCHREALTIME

# --- 3. Colors ---
teal="\033[38;5;116m"; blue="\033[38;5;153m"; mauve="\033[38;5;146m"
grey="\033[38;5;250m"; bg_dark="\033[48;5;116m"; bg_dark2="\033[48;5;146m"
fg_dark="\033[38;5;234m"; reset="\033[0m"

# --- 4. Help Menu ---
get_len() { echo -ne "$1" | sed 's/\x1b\[[0-9;]*m//g' | wc -m ; }
show_help() {
    local l_title=" ${teal}◖${bg_dark}${fg_dark} WildWater Shell Launcher ${reset}${teal}◗${reset} "
    local l_usage="  ${blue}Usage:${reset} $0 DATAFILE COMMAND [TYPE #ID]"
    # ... (rest of your help menu code remains the same)
    echo -e "Example: $0 data.dat leaks Unit #WJ100255G"
    exit 1
}

# --- 5. Argument Parsing ---
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

# IMPROVEMENT: Collect all arguments from index 2 onwards.
# This fixes the "Unit #ID" space issue even without quotes.
FULL_PARAM_STRING="${RAW_ARGS[*]:2}"

# Extract ID: Remove everything up to the last '#'
CLEAN_ID="${FULL_PARAM_STRING##*#}"

# Trim potential whitespace
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

# 7a. Data Sorting
if [[ "$USE_CACHE" == true ]]; then
    [[ ! -f "$TEMP_DIR/plants.dat" ]] && cp "$CACHE_DIR"/*.dat "$TEMP_DIR/"
else
    if [[ "$COMMAND" == "leaks" ]]; then
        # Ensure we found a hash; if not, CLEAN_ID might be the whole string
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

# 7b. Binary Execution
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

# --- 8. Cache & Timing ---
if [[ "$USE_CACHE" == false && "$COMMAND" != "leaks" ]]; then
    mkdir -p "$CACHE_DIR"
    cp "$TEMP_DIR"/*.dat "$CACHE_DIR/"
    echo "$DATAFILE" > "$CACHE_DIR/lfn"
fi

end_time=$EPOCHREALTIME
duration=$(echo "$end_time - $start_time" | bc)
ms_duration=$(echo "scale=0; ($duration * 1000) / 1" | bc)
echo -e "${blue}Total Time : ${ms_duration}ms${reset}"
