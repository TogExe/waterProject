set datafile separator ";"

set terminal pngcairo size 1400,700
set output "graphhigh-Real.png"

set yrange [5000:80000]
set style data histograms
set boxwidth 2
set style fill solid

set grid ytics
set xtics rotate by -90
set key off

set xlabel "Factory Name"
set ylabel "Real collected volume in M.m^3"
set title "Histogram of the maximum capacity of the 10 biggest factory"

cmd = sprintf("< tail -n +2 %s | sort -k2,2g -t ';' | tail -n 10", datafile)
plot cmd using ($3/maxval):xtic(1) title "Normalized"
