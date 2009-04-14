
.PHONY: all
.PHONY: clean
.PHONY: distclean
.PHONY: distdir
.PHONY: install
.PHONY: check

doc: article.pdf tutorial.pdf

article.pdf: article.tex
	pdflatex article.tex
	pdflatex article.tex

tutorial.pdf: tutorial.tex intro.tex general_remarks.tex simple_tasks.tex display.tex indent.tex reroot.tex clade.tex installing.tex def_clades.tex tree_axioms.tex tree_properties.tex 
	./make_figures.sh
	pdflatex tutorial.tex
	pdflatex tutorial.tex

clean:
	$(RM) *.{pdf,out,cmd,aux,log,dvi,toc} *_svg.svg

