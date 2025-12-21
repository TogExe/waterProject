#!/bin/sh

DATA="$2"


if [["$1" = "max"]] then
gnuplot -e "datafile='${DATA}'" graphhigh-Max.gp

gnuplot -e "datafile='${DATA}'" graphlow-Max.gp
fi


if [["$1" = "src"]] then
gnuplot -e "datafile='${DATA}'" graphlow-Src.gp

gnuplot -e "datafile='${DATA}'" graphhigh-Src.gp
fi

if [["$1" = "real"]] then
gnuplot -e "datafile='${DATA}'" graphlow-Real.gp

gnuplot -e "datafile='${DATA}'" graphhigh-Real.gp
fi



