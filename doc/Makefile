
.PHONY: all
.PHONY: clean
.PHONY: distclean
.PHONY: distdir
.PHONY: install
.PHONY: check

all: nwutils_tutorial.pdf
	
TEX_SOURCES= advanced_tasks.tex bootscanning.tex checking_consistency.tex clade.tex condense.tex def_clades.tex display.tex distance.tex ed.tex general_remarks.tex gen.tex indent.tex installing.tex intro.tex labels.tex match.tex newick_order.tex nodes_vs_depth.tex nwutils_tutorial.tex order.tex prune.tex python_lib.tex rename.tex reroot.tex simple_tasks.tex support.tex topology.tex tree_axioms.tex tree_properties.tex trim.tex

check:

nwutils_tutorial.pdf: $(TEX_SOURCES)

	./make_figures.sh
	latexmk --pdf nwutils_tutorial.tex

clean:
	$(RM) *.{out,cmd,aux,log,dvi,toc} *_svg.svg *_svg.pdf

