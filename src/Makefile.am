BUILT_SOURCES = newick_parser.h newick_scanner.c newick_parser.c \
		address_scanner.c address_parser.c

bin_PROGRAMS = nw_indent nw_display nw_clade nw_reroot nw_rename \
	       nw_condense nw_support nw_ed nw_topology nw_distance \
	       nw_labels nw_prune nw_order

HDR = canvas.h concat.h enode.h hash.h lca.h link.h list.h nodemap.h \
	node_pos_alloc.h node_set.h parser.h readline.h redge.h rnode.h \
	rnode_iterator.h simple_node_pos.h svg_graph.h text_graph.h \
	to_newick.h tree.h tree_editor_rnode_data.h common.h

nw_indent_SOURCES = indent.c indent_lex.l
nw_indent_LDADD = @LEXLIB@

# I mention all headers here, so they're included in the distro (otherwise this
# is not necessary, it builds just fine)
NW_CORE = newick_parser.y newick_scanner.l rnode.c list.c parser.c \
	  link.c tree.c redge.c nodemap.c hash.c $(HDR)

NW_OUT = to_newick.c concat.c

AM_YFLAGS = -d

newick_scanner.c: newick_scanner.l
	flex -onewick_scanner.c newick_scanner.l 

address_scanner.c: address_scanner.l
	flex -oaddress_scanner.c address_scanner.l 

nw_display_SOURCES = $(NW_CORE) display.c canvas.c text_graph.c \
		simple_node_pos.c svg_graph.c readline.c lca.c \
		node_pos_alloc.c
nw_display_LDADD = @LEXLIB@

nw_clade_SOURCES = $(NW_CORE) $(NW_OUT) subtree.c lca.c rnode_iterator.c
nw_clade_LDADD = @LEXLIB@ 

nw_reroot_SOURCES = $(NW_CORE) $(NW_OUT) reroot.c lca.c
nw_reroot_LDADD = @LEXLIB@

nw_rename_SOURCES = $(NW_CORE) $(NW_OUT) rename.c readline.c
nw_rename_LDADD = @LEXLIB@

nw_condense_SOURCES = $(NW_CORE) $(NW_OUT) condense.c
nw_condense_LDADD = @LEXLIB@

nw_support_SOURCES = $(NW_CORE) $(NW_OUT) support.c node_set.c
nw_support_LDADD = @LEXLIB@

nw_ed_SOURCES = $(NW_CORE) $(NW_OUT) address_scanner.l address_parser.y \
	tree_editor.c enode.c
nw_ed_LDADD = @LEXLIB@ -lm

nw_topology_SOURCES = $(NW_CORE) $(NW_OUT) topology.c
nw_topology_LDADD = @LEXLIB@

nw_distance_SOURCES = $(NW_CORE) distance.c simple_node_pos.c lca.c \
		      node_pos_alloc.c
nw_distance_LDADD = @LEXLIB@

nw_labels_SOURCES = $(NW_CORE) labels.c 
nw_labels_LDADD = @LEXLIB@

nw_prune_SOURCES = $(NW_CORE) $(NW_OUT) prune.c 
nw_prune_LDADD = @LEXLIB@

nw_order_SOURCES = $(NW_CORE) $(NW_OUT) order.c
nw_order_LDADD = @LEXLIB@

# Removes automatically generated C code (Lex & YACC)
#
clean-local:
	$(RM) newick_scanner.c newick_parser.c newick_parser.h \
	address_scanner.c address_parser.c address_parser.h  \
	indent_lex.c
