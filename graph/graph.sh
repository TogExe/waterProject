#!/bin/bash

DATA="$2"


if [[ "$1" = "max" ]]; then
gnuplot -e "datafile='${DATA}'" graphhigh-VolMax.gp

gnuplot -e "datafile='${DATA}'" graphlow-VolMax.gp
fi


if [[ "$1" = "src" ]]; then
gnuplot -e "datafile='${DATA}'" graphlow-Vol.gp

gnuplot -e "datafile='${DATA}'" graphhigh-Vol.gp
fi

if [[ "$1" = "real" ]]; then
gnuplot -e "datafile='${DATA}'" graphlow-Real.gp

gnuplot -e "datafile='${DATA}'" graphhigh-Real.gp
fi
