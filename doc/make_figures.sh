#!/bin/bash

# Some figures in the docs are the direct output of nw_* commands. This script
# runs them so that the docs are up-to-date and accurate.

# Commands for making figures are in files with suffixes in _txt (text graphs),
# _nw (Newick) or _svg (SVG). The command and its result can be referenced in
# LaTeX by the following commands:
#
# \verbatiminput{my_command_txt.cmd}
# \verbatiminput{my_command_txt.out}

# These just generate text

for cmd in *_{txt,nw} ; do
	if [ $cmd -nt $cmd.out ] ; then
		echo $cmd
		# prefix a '$' for inputting into LaTeX
		echo -n "$ "  > $cmd.cmd
		cat $cmd >> $cmd.cmd
		# run command, pipe through expand b/c LaTeX's verbatiminput does not
		# handle TABs correctly, fold because it may exceed page width.
		sh < $cmd | fold | expand > $cmd.out
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
