BUILT_SOURCES = newick_parser.h newick_scanner.c newick_parser.c \
		address_scanner.c address_parser.c

bin_PROGRAMS = nw_indent nw_display nw_clade nw_reroot nw_rename \
	       nw_collapse nw_bipart nw_ed nw_topology

nw_indent_SOURCES = indent.c indent_lex.l
nw_indent_LDADD = @LEXLIB@

NW_CORE = newick_parser.y newick_scanner.l rnode.c list.c parser.c \
	  link.c tree.c redge.c

NW_OUT = to_newick.c concat.c

AM_YFLAGS = -d

newick_scanner.c: newick_scanner.l
	flex -onewick_scanner.c newick_scanner.l 

address_scanner.c: address_scanner.l
	flex -oaddress_scanner.c address_scanner.l 

nw_display_SOURCES = $(NW_CORE) display.c canvas.c text_graph.c 
nw_display_LDADD = @LEXLIB@

nw_clade_SOURCES = $(NW_CORE) $(NW_OUT) subtree.c lca.c nodemap.c hash.c \
		   rnode_iterator.c
nw_clade_LDADD = @LEXLIB@ 

nw_reroot_SOURCES = $(NW_CORE) $(NW_OUT) reroot.c lca.c nodemap.c hash.c
nw_reroot_LDADD = @LEXLIB@

nw_rename_SOURCES = $(NW_CORE) $(NW_OUT) rename.c hash.c
nw_rename_LDADD = @LEXLIB@

nw_collapse_SOURCES = $(NW_CORE) $(NW_OUT) collapse.c
nw_collapse_LDADD = @LEXLIB@

nw_bipart_SOURCES = $(NW_CORE) $(NW_OUT) bipart.c hash.c node_set.c
nw_bipart_LDADD = @LEXLIB@

nw_ed_SOURCES = $(NW_CORE) $(NW_OUT) address_scanner.l address_parser.y \
	tree_editor.c enode.c
nw_ed_LDADD = @LEXLIB@ -lm

nw_topology_SOURCES = $(NW_CORE) $(NW_OUT) topology.c
nw_topology_LDADD = @LEXLIB@

