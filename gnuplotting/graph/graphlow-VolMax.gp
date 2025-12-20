set datafile separator ";"

set terminal pngcairo size 1400,700
set output "graphlow-VolMax.png"


set yrange [1.05:1.20]
set style data histograms
set boxwidth 2  
set style fill solid


set grid ytics
set xtics rotate by -90
set key off

set xlabel "Factory Name"
set ylabel "Vol Max in M.m^3"
set title "Graph of the maximum volume of the 50 smallest factory"


cmd = sprintf("< tail -n +2 %s | sort -k2,2g -t ';' | head -n 50", datafile)

plot cmd using 2:xtic(1)	

