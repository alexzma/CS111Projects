#! /usr/bin/gnuplot
#
# purpose:
#	 generate data reduction graphs for the multi-threaded list project
#
# input: lab2b_list.csv
#	1. test name
#	2. # threads
#	3. # iterations per thread
#	4. # lists
#	5. # operations performed (threads x iterations x (ins + lookup + delete))
#	6. run time (ns)
#	7. run time per operation (ns)
#
# output:
#	lab2_list-1.png ... cost per operation vs threads and iterations
#	lab2_list-2.png ... threads and iterations that run (un-protected) w/o failure
#	lab2_list-3.png ... threads and iterations that run (protected) w/o failure
#	lab2_list-4.png ... cost per operation vs number of threads
#
# Note:
#	Managing data is simplified by keeping all of the results in a single
#	file.  But this means that the individual graphing commands have to
#	grep to select only the data they want.
#
#	Early in your implementation, you will not have data for all of the
#	tests, and the later sections may generate errors for missing data.
#

# general plot parameters
set terminal png
set datafile separator ","

# Operations per second for number of threads
set title "List-1: Operations per Second for Number of Threads"
set xlabel "Number of Threads"
set xrange [0:]
set ylabel "Operations per Second"
set logscale y 10
set output 'lab2b_1.png'

plot \
     "< grep -e 'list-none-m,[0-9]*,1000,1,' lab2b_list.csv" using ($2):(1000000000/($7)) \
	title 'mutex' with linespoints lc rgb 'red', \
     "< grep -e 'list-none-s,[0-9]*,1000,1,' lab2b_list.csv" using ($2):(1000000000/($7)) \
	title 'spin' with linespoints lc rgb 'blue'

# Wait-for-Lock and Average Operation Time vs Number of Threads
set title "List-2: Wait-for-Lock Time and Average Operation Time vs Number of Threads"
set xlabel "Number of Threads"
set xrange [0:]
set ylabel "Time (ns)"
set logscale y 10
set output 'lab2b_2.png'
plot \
     "< grep -e 'list-none-m,[0-9]*,[0-9]*,1,' lab2b_list.csv" using ($2):($7) \
	title 'Average Operation Time' with linespoints lc rgb 'green', \
     "< grep -e 'list-none-m,[0-9]*,[0-9]*,1,' lab2b_list.csv" using ($2):($8) \
	title 'Wait-for-Lock Time' with linespoints lc rgb 'red'

# Success of multi-list program     
set title "List-3: Multi-List Iterations that Run without Failure"
set xrange [0:]
set xlabel "Number of Threads"
set ylabel "Number of Iterations"
set logscale y 10
set output 'lab2b_3.png'
plot \
    "< grep 'list-id-none' lab2b_list.csv" using ($2):($3) \
	with points lc rgb "red" title "Unprotected", \
    "< grep 'list-id-m' lab2b_list.csv" using ($2):($3) \
	with points pointtype 2 lc rgb "green" title "Mutex", \
    "< grep 'list-id-s' lab2b_list.csv" using ($2):($3) \
	with points pointtype 4 lc rgb "blue" title "Spin-Lock"

# Compare throughput vs number of threads with different numbers of lists
set title "List-4: Throughput vs Number of Threads for Mutex Lists"
set xrange [0:]
set xlabel "Number of Threads"
set ylabel "Throughput (Operations per Second)"
set logscale y 10
set output 'lab2b_4.png'
plot \
    "< grep -e 'list-none-m,[0-9]*,1000,1,' lab2b_list.csv" using ($2):(1000000000/($7)) \
	with linespoints lc rgb "red" title "1 List", \
    "< grep -e 'list-none-m,[0-9]*,1000,4' lab2b_list.csv" using ($2):(1000000000/($7)) \
	with linespoints lc rgb "blue" title "4 Lists", \
    "< grep -e 'list-none-m,[0-9]*,1000,8' lab2b_list.csv" using ($2):(1000000000/($7)) \
	with linespoints lc rgb "green" title "8 Lists", \
    "< grep -e 'list-none-m,[0-9]*,1000,16' lab2b_list.csv" using ($2):(1000000000/($7)) \
	with linespoints lc rgb "purple" title "16 Lists"

set title "List-5: Throughput vs Number of Threads for Spin Lists"
set xrange [0:]
set xlabel "Number of Threads"
set ylabel "Throughput (Operations per Second)"
set logscale y 10
set output 'lab2b_5.png'
plot \
    "< grep -e 'list-none-s,[0-9]*,1000,1,' lab2b_list.csv" using ($2):(1000000000/($7)) \
	with linespoints lc rgb "red" title "Spin 1 List", \
    "< grep -e 'list-none-s,[0-9]*,1000,4' lab2b_list.csv" using ($2):(1000000000/($7)) \
	with linespoints lc rgb "blue" title "Spin 4 List", \
    "< grep -e 'list-none-s,[0-9]*,1000,8' lab2b_list.csv" using ($2):(1000000000/($7)) \
	with linespoints lc rgb "green" title "Spin 8 List", \
    "< grep -e 'list-none-s,[0-9]*,1000,16' lab2b_list.csv" using ($2):(1000000000/($7)) \
	with linespoints lc rgb "purple" title "Spin 16 List"
