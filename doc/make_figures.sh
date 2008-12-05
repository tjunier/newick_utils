#!/bin/bash

# Some figures in the docs are teh direct output of nw_* commands. This script
# runs them so that the docs are up-to-date and accurate.

# Commands for making figures are in files with suffixes in _txt (text graphs),
# _nw (Newick) or _svg (SVG). The command and its result can be referenced in
# LaTeX by the following commands:
#
# \verbatiminput{my_command_txt.cmd}
# \verbatiminput{my_command_txt.out}

# These just generate text

for cmd in *_{txt,nw} ; do
	# prefix a '$' for inputting into LaTeX
	echo -n "$ "  > $cmd.cmd
	cat $cmd >> $cmd.cmd
	# run command
	sh < $cmd > $cmd.out
done

for cmd in *_svg ; do
	# prefix a '$' for inputting into LaTeX
	echo -n "$ "  > $cmd.cmd
	cat $cmd >> $cmd.cmd
	# run command
	sh < $cmd > $cmd.svg
	inkscape -f $cmd.svg -A $cmd.pdf
done
