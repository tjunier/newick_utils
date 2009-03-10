#!/bin/bash 

# bootscan.sh - an example of bootscanning using the Newick Utilities in
# cooperation with other command-line bioinformatics tools. Requires Muscle
# [1], EMBOSS [2], and PhyML [3]. Will create files (alignment, trees, etc) in
# the directory in which the input file is found.

# References
# [1] http://www.drive5.com/muscle/
# [2] http://emboss.sourceforge.net 
# [3] http://atgc.lirmm.fr/phyml/

shopt -s -o nounset

declare -r INPUT_FILE=$1
declare -r MUSCLE_OUT=$INPUT_FILE.mfa

# Align sequences

muscle -quiet -in $INPUT_FILE -out $MUSCLE_OUT

# Slice alignment



