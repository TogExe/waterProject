

# Set the data file separator to semicolon
set datafile separator ";"

# Set the title and labels
set title "Histogram of Usines (Value vs Losses)"
set xlabel "Usine ID"
set ylabel "Value"
set y2label "Losses"
set ytics nomirror
set y2tics

# Plot the data as a histogram
set style data histograms
set style fill solid border -1
set boxwidth 0.5

# Plot 'value' as bars and 'losses' as lines on the second y-axis
plot "data.txt" using 2:xtic(1) title "Value" axes x1y1, \
     "data.txt" using 3:xtic(1) title "Losses" axes x1y2 with linespoints
