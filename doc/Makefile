
.PHONY: all
.PHONY: clean
.PHONY: distclean
.PHONY: distdir
.PHONY: install
.PHONY: check

all:
	
check:

nwutils_tutorial.pdf: nwutils_tutorial.tex
	./make_figures.sh
	pdflatex nwutils_tutorial.tex
	pdflatex nwutils_tutorial.tex

clean:
	$(RM) *.{out,cmd,aux,log,dvi,toc} *_svg.svg *_svg.pdf

