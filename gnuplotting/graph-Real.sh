#!/bin/sh

DATA="histo_all.dat"

gnuplot -e "datafile='${DATA}'" graphlow-Real.gp

gnuplot -e "datafile='${DATA}'" graphhigh-Real.gp