set datafile separator ":"
set terminal png size 1200,300
set title "TeamB Statistics"
set ylabel "values"
set xlabel "Date"
set xdata time
set timefmt "%s"
set key left top
set grid
plot "timeseries.csv" using 1:12 with lines lw 0.3 title 'bestfit', \
 "timeseries.csv" using 1:13 with lines lw 0.3 title 'avgfit', \
 "timeseries.csv" using 1:15 with lines lw 0.3 title 'avgfriendly', \
 "timeseries.csv" using 1:16 with lines lw 0.3 title 'avgcrash', \
 "timeseries.csv" using 1:14 with lines lw 0.3 title 'avghits'
