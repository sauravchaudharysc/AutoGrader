#!/usr/local/bin/gnuplot -persist



set terminal pngcairo size 800,600 enhanced font 'Verdana,12'
set output './plots/Throughput_per_client.png'

set title 'Number of Clients vs. Throughput(req/s)'
set xlabel 'Number of Clients'
set ylabel 'Throughput(req/s)'

set style data linespoints


plot 'avgresponsevsM.txt' using 1:3 with linespoints title 'Throughput(req/s)'


