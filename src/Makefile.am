BUILT_SOURCES = newick_parser.h

bin_PROGRAMS = nw_indent nw_display nw_clade nw_reroot nw_rename nw_collapse

nw_indent_SOURCES = indent.c indent_lex.l
nw_indent_LDADD = @LEXLIB@

NW_CORE = newick_parser.y newick_scanner.l rnode.c list.c parser.c link.c tree.c redge.c

NW_OUT = to_newick.c concat.c

AM_YFLAGS = -d

nw_display_SOURCES = $(NW_CORE) display.c canvas.c text_graph.c 
nw_display_LDADD = @LEXLIB@ -lm

nw_clade_SOURCES = $(NW_CORE) $(NW_OUT) subtree.c lca.c nodemap.c
nw_clade_LDADD = @LEXLIB@ -lm

nw_reroot_SOURCES = $(NW_CORE) $(NW_OUT) reroot.c lca.c nodemap.c
nw_reroot_LDADD = @LEXLIB@ -lm

nw_rename_SOURCES = $(NW_CORE) $(NW_OUT) rename.c hash.c
nw_rename_LDADD = @LEXLIB@ -lm

nw_collapse_SOURCES = $(NW_CORE) $(NW_OUT) collapse.c
nw_collapse_LDADD = @LEXLIB@ -lm
