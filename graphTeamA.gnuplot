set datafile separator ":"
set terminal png size 1200,300
set title "TeamA Statistics"
set ylabel "values"
set xlabel "Date"
set xdata time
set timefmt "%s"
set key left top
set grid
plot "timeseries.csv" using 1:2 with lines lw 0.3 title 'bestfit', \
 "timeseries.csv" using 1:3 with lines lw 0.3 title 'avgfit', \
 "timeseries.csv" using 1:5 with lines lw 0.3 title 'friendly', \
 "timeseries.csv" using 1:8 with lines lw 0.3 title 'ammo', \
 "timeseries.csv" using 1:4 with lines lw 0.3 title 'avghits'
