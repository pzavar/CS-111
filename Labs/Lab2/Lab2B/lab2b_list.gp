#! /usr/bin/gnuplot
#NAME: Parham Hajzavar
#EMAIL: parhamzavar@gmail.com
#ID: 405181239
#
#purpose: generating data reduction graphs for the multi-threaded list project part B
#
#input: lab2b_list.csv
#
#output: lab2b_1.png lab2b_2.png lab2b_3.png lab2b_4.png lab2b_5.png
#
#
#
#general plot parameters
set terminal png
set datafile separator ","


#generating lab2b_1.png
set title "Throughput vs. Number of Threads"
set xlabel "Number of Threads"
set logscale x 2
set xrange[0.75:]
set ylabel "Throughput (op/s)"
set logscale y 10
set output 'lab2b_1.png'

plot \
     "< grep -e 'list-none-s,[0-9]*,1000,1,' lab2b_list.csv" using ($2):(1000000000/($7)) \
     title 'with spin' with linespoints lc rgb 'red', \
     "< grep -e 'list-none-m,[0-9]*,1000,1,' lab2b_list.csv" using ($2):(1000000000/($7)) \
     title 'with mutex' with linespoints lc rgb 'blue'


#generating lab2b_2.png
set title "Mean Time per Mutex Wait & Mean Time per Operation"
set xlabel "Number of Threads"
set logscale x 2
set xrange[0.75:]
set ylabel "Time (ns)"
set logscale y 10
set output 'lab2b_2.png'

plot \
     "< grep -e 'list-none-m,[0-9]*,1000,1,' lab2b_list.csv" using ($2):($7) \
     title 'Time/Operation' with linespoints lc rgb 'red', \
     "< grep -e 'list-none-m,[0-9]*,1000,1,' lab2b_list.csv" using ($2):($8) \
     title 'Average Wait Time w/ Mutex' with linespoints lc rgb 'blue'


#generating lab2b_3.png
set title "Successful Iterations vs. Threads"
set xlabel "Number of Threads"
set logscale x 2
set xrange [0.75:]
set ylabel "Successful Iterations"
set logscale y 10
set output 'lab2b_3.png'
plot \
	"< grep 'list-id-none' lab2b_list.csv | grep '^list'" using ($2):($3) \
        with points lc rgb 'red' title 'no_sync', \
	"< grep list-id-m lab2b_list.csv | grep '^list'" using ($2):($3) \
        with points lc rgb 'blue' title 'with_mutex', \
	"< grep list-id-s lab2b_list.csv | grep '^list'" using ($2):($3) \
        with points lc rgb 'orange' title 'with_spin' 


#generating lab2b_4.png
set title "Throughput vs. Number of Threads"
set xlabel "Number of Threads"
set logscale x 2
set xrange[0.75:]
set ylabel "Throughput"
set logscale y 10
set output 'lab2b_4.png'
plot \
     "< grep -e 'list-none-m,[0-9]*,1000,1,' lab2b_list.csv" using ($2):(1000000000/($7)) \
     title '1 list' with linespoints lc rgb 'red', \
     "< grep -e \'list-none-m,[0-9]*,1000,4,\' lab2b_list.csv" using ($2):(1000000000/($7)) \
     title '4 lists' with linespoints lc rgb 'blue', \
     "< grep -e \'list-none-m,[0-9]*,1000,8,\' lab2b_list.csv" using ($2):(1000000000/($7)) \
     title '8 lists' with linespoints lc rgb 'green', \
     "< grep -e \'list-none-m,[0-9]*,1000,16,\' lab2b_list.csv" using ($2):(1000000000/($7)) \
     title '16 lists' with linespoints lc rgb 'pink'


#generating lab2b_5.png
set title "Throughput vs. Number of Threads"
set xlabel "Number of Threads"
set logscale x 2
set xrange [0.75:]
set ylabel "Throughput"
set logscale y 10
set output 'lab2b_5.png'
plot \
     "< grep 'list-none-s,.*,1000,1,' lab2b_list.csv" using ($2):(1000000000/($7)) \
     title '1' with linespoints lc rgb 'red', \
     "< grep 'list-none-s,.*,1000,4' lab2b_list.csv" using ($2):(1000000000/($7)) \
     title '4' with linespoints lc rgb 'blue', \
     "< grep 'list-none-s,.*,1000,8' lab2b_list.csv" using ($2):(1000000000/($7)) \
     title '8' with linespoints lc rgb 'green', \
     "< grep 'list-none-s,.*,1000,16' lab2b_list.csv" using ($2):(1000000000/($7)) \
     title '16' with linespoints lc rgb 'orange'