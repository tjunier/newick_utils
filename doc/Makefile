
.PHONY: all
.PHONY: clean

all: article.pdf tutorial.pdf

article.pdf: article.tex
	pdflatex article.tex

tutorial.pdf: tutorial.tex
	./make_figures.sh
	pdflatex tutorial.tex

clean:
	$(RM) *.{pdf,out,cmd,aux,log,dvi,toc,svg}

