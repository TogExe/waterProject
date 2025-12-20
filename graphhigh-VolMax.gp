set datafile separator ";"

set terminal pngcairo size 1400,700
set output "graphhigh-VolMax.png"

set yrange [0:5000000]
set style data histograms
set boxwidth 2
set style fill solid


set grid ytics
set xtics rotate by -90
set key off

set xlabel "Factory Name"
set ylabel "Volume in M.m^3"
set title "Histogram of the maximum capacity of the 10 biggest factory"

cmd = sprintf("< tail -n +2 %s | sort -k2,2g -t ';' | tail -n 10", datafile)

plot cmd using 2:xtic(1)
