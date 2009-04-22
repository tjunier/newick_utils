#!/bin/bash

# nodes_vs_depth.sh - plots the number of nodes as a function of depth.

shopt -s -o nounset

declare -r TREE=$1
declare -ri STEPS=$2
declare -r DATA=$1.cvd
declare -r PLOT=$1.plt
declare -r GRAPHICS=$1.eps

# Find tree's depth, i.e. the length of the longest branch. The increment is
# this value divided by th enumber of steps. 
depth=$(nw_distance $TREE | sort -n | tail -1)
increment=$(echo "scale=4; $depth/$STEPS" | bc)

# Now output the number of clades at all sampled depths
for depth in $(seq 0 $increment $depth ) ; do
	nb_clades_at_depth=$(nw_ed -n -o $TREE "d >= $depth" s | wc -l)
	printf "%g\t%d\n" $depth $nb_clades_at_depth
done > $DATA

sum_heights=$(tac $DATA | awk 'NR > 1 {sum +=$2} END {print sum}')
abs_area=$(echo "scale=2; $sum_heights * $increment" | bc)
# count leaves -- later add a nw_util for tree info and statistics
num_leaves=$(nw_indent $TREE | grep -v '[()]' | wc -l)
rel_area=$(echo "scale=2; $abs_area / ($depth * $num_leaves)" | bc)

# Print GNUPlot code
cat <<END > $PLOT
set terminal postscript color eps "Helvetica" 18
set output '$GRAPHICS'
set title 'Number of Nodes as a function of Depth in $TREE'
set xlabel 'Tree Depth'
set ylabel '# Nodes'
set border back
set label "normalized area: $rel_area      " at $depth,1.5 right front
plot [] [1:] '$DATA' with steps linewidth 4 title ''
END

# Produce plot
gnuplot < $PLOT
