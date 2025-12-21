#!/bin/bash

SCRIPT_PATH="$(readlink -f "$0")"
SCRIPT_DIR="${SCRIPT_PATH%/*}"

DATA="$2"

GRAPH="$PWD/"

cd "$GRAPH"

if [[ "$1" = "max" ]]; then
gnuplot -e "datafile='${DATA}'" graphhigh-VolMax.gp

gnuplot -e "datafile='${DATA}'" graphlow-VolMax.gp
fi


if [[ "$1" = "src" ]]; then
gnuplot -e "datafile='${DATA}'" graphlow-Src.gp

gnuplot -e "datafile='${DATA}'" graphhigh-Src.gp
fi

if [[ "$1" = "real" ]]; then
gnuplot -e "datafile='${DATA}'" graphlow-Real.gp

gnuplot -e "datafile='${DATA}'" graphhigh-Real.gp
fi
