#!/bin/sh

DATA="histo_all.dat"


if [["$1" = "max"]] then
gnuplot -e "datafile='${DATA}'" graphhigh-Vol+Real.gp

gnuplot -e "datafile='${DATA}'" graphlow-Vol+Real.gp
fi


if [["$1" = "vol"]] then
gnuplot -e "datafile='${DATA}'" graphlow-Vol.gp

gnuplot -e "datafile='${DATA}'" graphhigh-Vol.gp
fi

if [["$1" = "real"]] then
gnuplot -e "datafile='${DATA}'" graphlow-Real.gp

gnuplot -e "datafile='${DATA}'" graphhigh-Real.gp
fi



