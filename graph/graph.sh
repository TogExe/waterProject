#!/bin/bash
SCRIPT_PATH="$(readlink -f "$0")"
SCRIPT_DIR="${SCRIPT_PATH%/*}"


DATA="$2"

GRAPH="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$GRAPH"


if [[ "$1" = "max" ]]; then
gnuplot -e "datafile='${DATA}'" graphhigh-VolMax.gp > /dev/null 2>&1

gnuplot -e "datafile='${DATA}'" graphlow-VolMax.gp > /dev/null 2>&1
fi


if [[ "$1" = "src" ]]; then
gnuplot -e "datafile='${DATA}'" graphlow-Src.gp > /dev/null 2>&1

gnuplot -e "datafile='${DATA}'" graphhigh-Src.gp > /dev/null 2>&1
fi

if [[ "$1" = "real" ]]; then
gnuplot -e "datafile='${DATA}'" graphlow-Real.gp > /dev/null 2>&1

gnuplot -e "datafile='${DATA}'" graphhigh-Real.gp > /dev/null 2>&1
fi
