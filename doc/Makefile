
.PHONY: all
.PHONY: clean
.PHONY: distclean
.PHONY: distdir
.PHONY: install
.PHONY: check

all: nwutils_tutorial.pdf
	
%_txt.cmd : %_txt
	echo -n '$ ' > $@	
	cat $< >> $@
	sh < $< 2>&1 | fold | expand > $*.out

check:

nwutils_tutorial.pdf: *.tex
	./make_figures.sh
	latexmk --pdf nwutils_tutorial.tex

clean:
	$(RM) *.{out,cmd,aux,log,dvi,toc} *_svg.svg *_svg.pdf

