
.PHONY: all
.PHONY: clean
.PHONY: clean-images
.PHONY: distclean
.PHONY: distdir
.PHONY: install
.PHONY: check

all: nwutils_tutorial.pdf
	
################################################################
# Implicit rules 

# The *.cmd files are command lines prefixed with a '$ ', to appear literally
# in the manual as what the user types in the shell. The *.{nw,txt,pdf} files
# are the commands' output, integrated into the manual as text (nw,txt) or
# graphics (pdf).

%_txt.cmd : %_txt
	echo -n '$$ ' > $@	
	cat $< >> $@

%_txt.out : %_txt
	sh < $< 2>&1 | fold | expand > $@

%_nw.cmd : %_nw
	echo -n '$$ ' > $@	
	cat $< >> $@

%_nw.out : %_nw
	sh < $< 2>&1 | fold | expand > $@

%_svg.cmd : %_svg
	echo -n '$$ ' > $@
	cat $< >> $@

%_svg.pdf : %_svg
	sh < $< > $<.svg
	inkscape -f $<.svg -A $@

################################################################
# Variables

# Some scripts (e.g. bootscan.sh) are found in ../src (they are not installed
# by make install). By putting ../src *before* the PATH, we ensure that the
# version of newick utils used is the latest compiled one.

export PATH := ../src/:$(PATH)	

regular_svg_commands = $(wildcard *_svg)
regular_svg_command_strings = $(addsuffix .cmd, $(regular_svg_commands))
regular_pdfs = $(addsuffix .pdf, $(regular_svg_commands))
newick_commands = $(wildcard *_nw)
newick_command_strings = $(addsuffix .cmd, $(newick_commands))
newick_command_outputs = $(addsuffix .out, $(newick_commands))
text_commands = $(wildcard *_txt)
text_command_strings = $(addsuffix .cmd, $(text_commands))
text_command_outputs = $(addsuffix .out, $(text_commands))

homino = homino_0 homino_1 homino_2 homino_3 homino_4 homino_5 homino_6
homino_pdfs = $(addsuffix .pdf, $(homino))
homino_svgs = $(addsuffix .svg, $(homino))

nodes_vs_depth_pdfs = star.pdf balanced.pdf short_leaves.pdf

################################################################
# Targets

# Empty, but needed (otherwise 'make check' will complain)
check:

# to show the value of variables (debugging)
show:
	echo $(PATH)

nwutils_tutorial.pdf: *.tex *.bib \
	$(regular_pdfs) css.map \
	$(regular_svg_command_strings) \
	$(newick_command_outputs) \
	$(newick_command_strings) \
	$(text_command_strings) \
	$(text_command_outputs) \
	$(homino_pdfs) $(nodes_vs_depth_pdfs) bootscan_1.pdf
	pdflatex nwutils_tutorial.tex
	bibtex   nwutils_tutorial.aux
	pdflatex nwutils_tutorial.tex
	pdflatex nwutils_tutorial.tex

HRV_3UTR.dna.bscan.eps:
	bootscan.sh HRV_3UTR.dna HRV-93 CL073908

bootscan_1.pdf: HRV_3UTR.dna.bscan.eps
	epstopdf HRV_3UTR.dna.bscan.eps --outfile $@

$(nodes_vs_depth_pdfs) : %.pdf : %
	nodes_vs_depth.sh $< 40
	epstopdf $*.eps

$(homino_svgs) : hominoidea.nw
	nw_display -s -w 300 -v 30 -l 'font-size:14;font-style:italic' \
	hominoidea.nw | csplit -sz -f homino_ -b '%d.svg' - '/<?xml/' {*}

$(homino_pdfs) : %.pdf : %.svg
	inkscape -f $< -A $@

# removes auxiliary and intermediary files (including SVG and EPS)
clean:
	$(RM) $(text_command_outputs) $(newick_command_outputs) $(text_command_strings) $(regular_svg_command_strings) $(newick_command_strings) *.aux *.log *.dvi *.toc *.eps *.bbl *.blg *_svg.svg HRV_ingrp.svg

# like clean, plus removes all PDFs except the main target
clean-images: clean
	$(RM) *_svg.pdf $(homino_pdfs) $(homino_svgs) $(nodes_vs_depth_pdfs) bootscan_1.pdf

# leaves a blank slate to start from scratch
distclean: clean-images
	$(RM) nwutils_tutorial.pdf
