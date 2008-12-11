
.PHONY: all
.PHONY: clean
.PHONY: distclean
.PHONY: distdir

all: article.pdf tutorial.pdf

article.pdf: article.tex
	pdflatex article.tex
	pdflatex article.tex

tutorial.pdf: tutorial.tex
	./make_figures.sh
	pdflatex tutorial.tex
	pdflatex tutorial.tex

clean:
	$(RM) *.{pdf,out,cmd,aux,log,dvi,toc,svg}

