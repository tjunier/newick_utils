#!/bin/bash

# plateau.sh - computes the 'plateau function' for a phylogram. By 'plateau
# function' I mean the number of clades as a function of tree depth.

shopt -s -o nounset

declare -ri STEPS=10

declare -r TREE=$1

# find tree's depth, i.e. the length of the longest branch
depth=$(nw_distance $TREE | sort -n | tail -1)
increment=$(echo "scale=4; $depth/$STEPS" | bc)
seq $increment $increment $depth
