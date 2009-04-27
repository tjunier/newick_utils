
.PHONY: all
.PHONY: clean
.PHONY: distclean
.PHONY: distdir
.PHONY: install
.PHONY: check

TEX_SOURCES = advanced_tasks.tex bootscanning.tex clades_vs_depth.tex clade.tex condense.tex def_clades.tex display.tex distance.tex general_remarks.tex higher_lvl.tex indent.tex installing.tex intro.tex labels.tex prune.tex reroot.tex simple_tasks.tex support.tex topology.tex tree_axioms.tex tree_properties.tex tutorial.tex

all: tutorial.pdf

tutorial.pdf: $(TEX_SOURCES)
	./make_figures.sh
	pdflatex tutorial.tex
	pdflatex tutorial.tex

clean:
	$(RM) *.{out,cmd,aux,log,dvi,toc,eps,plt,cvd} *_svg.svg *_svg.pdf

