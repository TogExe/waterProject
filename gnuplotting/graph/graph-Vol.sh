#!/bin/sh

DATA="histo_all.dat"

gnuplot -e "datafile='${DATA}'" graphlow-Vol.gp

gnuplot -e "datafile='${DATA}'" graphhigh-Vol.gp