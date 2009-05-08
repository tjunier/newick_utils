#!/bin/bash

# Some figures in the docs are the direct output of nw_* commands. This script
# runs them so that the docs are up-to-date and accurate.

# Commands for making figures are in files with suffixes in _txt (text graphs),
# _nw (Newick) or _svg (SVG). The command and its result can be referenced in
# LaTeX by the following commands:
#
# \verbatiminput{my_command_txt.cmd}
# \verbatiminput{my_command_txt.out}

PATH=../src:$PATH # try the latest versions of the utils first

# These just generate text

for cmd in *_{txt,nw} ; do
	if [ $cmd -nt $cmd.out ] ; then
		echo $cmd
		# prefix a '$' for inputting into LaTeX
		echo -n "$ "  > $cmd.cmd
		cat $cmd >> $cmd.cmd
		# run command, pipe through expand b/c LaTeX's verbatiminput does not
		# handle TABs correctly, fold because it may exceed page width.
		sh < $cmd 2>&1 | fold | expand > $cmd.out
	fi
done

# These generate PDF from SVG

for cmd in *_svg ; do
	if [ $cmd -nt $cmd.svg ]; then
		echo $cmd
		# prefix a '$' for inputting into LaTeX
		echo -n "$ "  > $cmd.cmd
		cat $cmd >> $cmd.cmd
		# run command
		sh < $cmd > $cmd.svg
		inkscape -f $cmd.svg -A $cmd.pdf
	fi
done

# Special tasks

# These run ../src/nodes_vs_depth.sh on the relevant files

for nvsd in star balanced short_leaves; do
	if [ ../src/nodes_vs_depth.sh -nt $nvsd.eps ]; then
		echo $nvsd.eps
		../src/nodes_vs_depth.sh $nvsd 40
		epstopdf $nvsd.eps
	fi
done

# These make files from a multiple SVG
if [ hominoidea.nw -nt homino_0.pdf ]; then
	nw_display -s -w 300 -v 30  -l 'font-size:14;font-style:italic' \
	hominoidea.nw | csplit -sz -f homino_ -b '%d.svg' - '/<?xml/' {*}
	for homsvg in homino_?.svg ; do
		echo $homsvg
		inkscape -f $homsvg -A ${homsvg/svg/pdf}
	done
fi
