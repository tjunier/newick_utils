
.PHONY: all
.PHONY: clean
.PHONY: distclean
.PHONY: distdir
.PHONY: install
.PHONY: check

all:
	
check:


tutorial.pdf: tutorial.tex
	./make_figures.sh
	pdflatex tutorial.tex
	pdflatex tutorial.tex

clean:
	$(RM) *.{out,cmd,aux,log,dvi,toc} *_svg.svg *_svg.pdf

