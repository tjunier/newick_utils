#!/bin/bash

# nodes_vs_depth.sh - plots the number of nodes as a function of depth.

shopt -s -o nounset

declare -ri STEPS=10

declare -r TREE=$1
declare -r DATA=$1.cvd
declare -r PLOT=$1.plt
declare -r GRAPHICS=$1.png

# Find tree's depth, i.e. the length of the longest branch. The increment is
# this value divided by th enumber of steps. 
depth=$(nw_distance $TREE | sort -n | tail -1)
increment=$(echo "scale=4; $depth/$STEPS" | bc)

# Now output the number of clades at all sampled depths
for depth in $(seq 0 $increment $depth ) ; do
	nb_clades_at_depth=$(nw_ed -n -o $TREE "d >= $depth" s | wc -l)
	printf "%g\t%d\n" $depth $nb_clades_at_depth
done > $DATA

# Print GNUPlot code
cat <<END > $PLOT
set terminal png
set output '$GRAPHICS'
set title 'Number of Clades as a function of Depth in $TREE'
set xlabel 'Tree Depth'
set ylabel '# Clades'
plot [] [0:] '$DATA' with lines title ''
END

# Produce plot
gnuplot < $PLOT
