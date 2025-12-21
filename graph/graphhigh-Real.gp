set datafile separator ";"

set terminal pngcairo size 1400,700
set output "graphhigh-Real.png"

set style data histograms
set boxwidth 2
set style fill solid
set grid ytics
set xtics rotate by -90
set key off

set xlabel "Factory Name"
set ylabel "Real collected volume in M.m^3"
set title "Histogram of the maximum capacity of the 10 biggest factory"

# Compute max value in column 3 of the last 10 rows (after skipping header)
stats "< tail -n +2 ".datafile." | sort -k2,2g -t ';' | tail -n 10" using 3 nooutput
maxval = STATS_max

# Now plot values normalized to the max
cmd = sprintf("< tail -n +2 %s | sort -k2,2g -t ';' | tail -n 10", datafile)
plot cmd using ($3/maxval):xtic(1) title "Normalized"
