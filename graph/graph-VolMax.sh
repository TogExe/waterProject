#!/bin/sh

DATA="max_histogram_20251220_161751.csv"

gnuplot -e "datafile='${DATA}'" graphhigh-Vol+Real.gp

gnuplot -e "datafile='${DATA}'" graphlow-Vol+Real.gp