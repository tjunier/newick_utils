
.PHONY: all
.PHONY: clean
.PHONY: distclean
.PHONY: distdir

doc: article.pdf tutorial.pdf

article.pdf: article.tex
	pdflatex article.tex
	pdflatex article.tex

tutorial.pdf: tutorial.tex tree_axioms.tex tree_properties.tex
	./make_figures.sh
	pdflatex tutorial.tex
	pdflatex tutorial.tex

clean:
	$(RM) *.{pdf,out,cmd,aux,log,dvi,toc}

