#!/bin/bash 

# Thomas Junier, 2009

# bootscan.sh - an example of bootscanning using the Newick Utilities in
# cooperation with other command-line bioinformatics tools.

# To try it out, set your PATH (see below), go to ../data and do:
# ../src/bootscan.sh HRV_3UTR.dna HRV-93 CL073908

# The purpose of the program is to find recombination breakpoints in a sequence
# (hereafter called the 'reference') by comparing it to related sequences over
# an alignment. If the reference's nearest neighbor changes, then there is
# evidence of recombination.

# The program takes a multiple sequence file (FastA, unaligned), the ID of the
# outgroup, and the ID of the reference. Then i) it aligns the sequences; ii)
# it slices the alignments into windows ('slices'); iii) it computes a tree for
# each window. Then it makes a plot. In the plot, the distance (along the tree
# branches) between the reference and all other sequences is plotted against
# alignment position. This is similar to the classical bootscan, which plotted
# percent identity. If the sequence with smallest distance changes drastically,
# you may have a breakpoint.

# Requires Muscle [1], EMBOSS [2], PhyML 3.0 [3], and GNUPlot [5]; as
# well as the GNU core utilities [4] (which you probably already have if you're
# running Linux). The PATH should be set so that these programs are found, as
# the script cannot use absolute pathnames (for portability).

# Will create files (alignment, trees, etc) in the directory in which the input
# file is found (temporary files will be cleaned up).

# References
# [1] http://www.drive5.com/muscle
# [2] http://emboss.sourceforge.net 
# [3] http://atgc.lirmm.fr/phyml or apt-get in Ubuntu
# [4] http://www.gnu.org/software/coreutils
# [5] http://www.gnuplot.info

shopt -s -o nounset

################################################################
# Functions

slice_alignment()
{
	declare -r ALN_LENGTH=$(infoalign -only -alignlength $MUSCLE_OUT | head -1)
	for slice_start in $(seq 1 $SLICE_STEP $((ALN_LENGTH - SLICE_WIDTH))) ; do
		slice_stop=$((slice_start+SLICE_WIDTH-1))
		seqret -osformat phylip $MUSCLE_OUT[$slice_start:$slice_stop] \
			> ${MUSCLE_OUT}_slice_${slice_start}-${slice_stop}.phy
	done
}

# NOTE: the PhyML parameters in this function are geared towards fast (but
# lower quality) trees, to get short runtimes for purposes of demonstration.
# For more realistic trees, the runtimes can be much longer, and then you would
# probably run each job in parallel on a computing cluster, then wait for all
# jobs to finish before proceeding.

make_trees()
{
	for slice in ${MUSCLE_OUT}_slice_*.phy; do
		echo " $slice"
		phyml -i $slice -b 0 -o n > /dev/null
	done
}

reroot_trees()
{
	for unrooted_tree in  ${MUSCLE_OUT}_slice_*.phy_phyml_tree.txt; do
		nw_reroot $unrooted_tree $OUTGROUP > ${unrooted_tree/.txt/.rr.nw}
	done
}

# This extract the following data
# - a list of all labels
# - the number of labels
# - the index of the reference in the list of labels
# - the list of all labels _except_ the reference

label_data()
{
	first_tree=$(ls ${MUSCLE_OUT}_slice*.rr.nw | head -1)
	labels=($(nw_clade -s $first_tree $OUTGROUP | nw_labels -I -))
	nb_labels=${#labels[*]}
	# This gives the index of the reference in the list of labels (starting at 1)
	ref_ndx=$(echo ${labels[*]} | tr ' ' "\n" | awk -vref=$REFERENCE '$1 == ref {print NR}')
	labels_noref=($(echo ${labels[*]} | tr ' ' "\n" | grep -v $REFERENCE))
}

# This function prints the distance from the reference to all labels in the
# list, for each position. At the end of the loop, the header column and
# reference column are removed, and the lines are sorted by position.

extract_distances_noref()
{
	# The reference column will be 1+reference index (after the line header is removed)
	ref_col=$((ref_ndx+1))	
	for rooted_tree in ${MUSCLE_OUT}_slice_*.rr.nw ; do 
		position=${rooted_tree/*_slice_/}
		position=${position/-*}
		echo -n "$position	"	# TAB!
		# We compute a matrix of all-vs-all distances, but only keep the line for
		# reference vs. all
		nw_distance -mm -n $rooted_tree ${labels[*]} | sed -n '2,$p' | grep $REFERENCE
	done | cut -f1,3- | cut -f 1-$((ref_col-1)),$((ref_col+1))- | sort -k1n > $DIST_NOREF
}

plot_classic()
{
	#printf "set size 1,0.7\n" > $GNUPLOT_CODE 
	printf "set terminal postscript eps color solid linewidth 3 size 12cm,5.5cm 18\n" >> $GNUPLOT_CODE
	printf "set output '%s'\n" $BSCAN_IMAGE >> $GNUPLOT_CODE
	printf "set key outside\n" >> $GNUPLOT_CODE 
	printf "set title 'Bootscanning of %s WRT %s, slice size %d nt'\n" \
		$INPUT_FILE $REFERENCE $SLICE_WIDTH >> $GNUPLOT_CODE
	printf "set xlabel 'position of slice centre in alignment [nt]'\n" >> $GNUPLOT_CODE
	printf "set ylabel 'distance to reference [subst./site]'\n" >> $GNUPLOT_CODE
	printf "plot '%s' using (\$1+(%d/2)):2 with lines linewidth $LINE_WIDTH title '%s'" $DIST_NOREF $SLICE_WIDTH ${labels_noref[0]} >> $GNUPLOT_CODE
	for i in $(seq 2 $((nb_labels-1))); do
		printf ", '' using (\$1+(%d/2)):%d with lines linewidth $LINE_WIDTH title '%s'" $SLICE_WIDTH $((i+1)) ${labels_noref[$((i-1))]}
	done >> $GNUPLOT_CODE

	gnuplot $GNUPLOT_CODE
}

cleanup()
{
	rm $MUSCLE_OUT $DIST_NOREF $GNUPLOT_CODE
	rm ${MUSCLE_OUT}_slice_*.phy*
}

################################################################
# Parameters

if [ $# != 3 ] ; then
	echo "Usage: $0 <alignment> <outgroup ID> <reference ID>" >&2
	exit 1
fi

# General Parameters
declare -r INPUT_FILE=$1
declare -r OUTGROUP=$2
declare -r REFERENCE=$3
# Alignment options & params
declare -r MUSCLE_OUT=$INPUT_FILE.mfa
# Slicing options
declare -ri SLICE_WIDTH=300	# residues
declare -ri SLICE_STEP=50	# slice every SLICE_STEP residues
# Distance options & params
declare -r DIST_NOREF=$INPUT_FILE.nrdist
# Plotting options & params
declare -r GNUPLOT_CODE=$INPUT_FILE.dist.plt
declare -r BSCAN_IMAGE=$INPUT_FILE.bscan.eps
declare -ri LINE_WIDTH=2

################################################################
# Main

export EMBOSS_FILTER=true

echo "Aligning"
muscle -quiet -in $INPUT_FILE -out $MUSCLE_OUT
echo "Slicing alignment"
slice_alignment
echo "Computing trees"
make_trees
echo "Rerooting trees on $OUTGROUP"
reroot_trees

label_data

echo "Extracting distances for ${labels[*]}"
extract_distances_noref
printf "Plotting bootscan, output is %s\n" $BSCAN_IMAGE
plot_classic
echo "Cleaning up"
cleanup # comment this to see the temporary results
