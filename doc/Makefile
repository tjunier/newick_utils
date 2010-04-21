
.PHONY: all
.PHONY: clean
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
# Targets

# Empty, but needed (otherwise 'make check' will complain)
check:

nwutils_tutorial.pdf: *.tex
	./make_figures.sh
	latexmk --pdf nwutils_tutorial.tex

clean:
	$(RM) *.{out,cmd,aux,log,dvi,toc} *_svg.svg *_svg.pdf \
		homino_?.{svg,pdf}

nodes_vs_depth_pdfs = star.pdf balanced.pdf short_leaves.pdf

$(nodes_vs_depth_pdfs) : %.pdf : %
	../src/nodes_vs_depth.sh $< 40
	epstopdf $*.eps

homino_pdfs = homino_0.pdf homino_1.pdf homino_2.pdf homino_3.pdf homino_4.pdf homino_5.pdf homino_6.pdf

homino_svgs = homino_0.svg homino_1.svg homino_2.svg homino_3.svg homino_4.svg homino_5.svg homino_6.svg

$(homino_svgs) : hominoidea.nw
	nw_display -s -w 300 -v 30 -l 'font-size:14;font-style:italic' \
	hominoidea.nw | csplit -sz -f homino_ -b '%d.svg' - '/<?xml/' {*}

$(homino_pdfs) : %.pdf : %.svg
	inkscape -f $< -A $@
