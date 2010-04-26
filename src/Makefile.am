# These sources are built automatically (by lex and yacc)
BUILT_SOURCES = newick_parser.h newick_scanner.c newick_parser.c \
		address_scanner.c address_parser.c

bin_PROGRAMS = nw_indent nw_display nw_clade nw_reroot nw_rename \
	       nw_condense nw_support nw_ed nw_topology nw_distance \
	       nw_labels nw_prune nw_order nw_match nw_gen nw_trim \
	       nw_duration nw_stats #nw_toy disabled for release

# I mention all headers here, so they're included in the distro (otherwise this
# is not necessary, it builds just fine)
HDR = canvas.h concat.h enode.h error.h hash.h lca.h link.h list.h nodemap.h \
	node_pos_alloc.h node_set.h parser.h readline.h rnode.h \
	rnode_iterator.h simple_node_pos.h svg_graph.h text_graph.h \
	to_newick.h tree.h tree_editor_rnode_data.h common.h order_tree.h \
	tree_models.h xml_utils.h graph_common.h svg_graph_common.h \
	svg_graph_radial.h svg_graph_ortho.h masprintf.h

NW_CORE = newick_parser.y newick_scanner.l rnode.c list.c parser.c \
	link.c tree.c nodemap.c hash.c rnode_iterator.c \
	masprintf.c to_newick.c concat.c lca.c error.c $(HDR)


AM_YFLAGS = -d

newick_scanner.c: newick_scanner.l
	flex -onewick_scanner.c newick_scanner.l 

address_scanner.c: address_scanner.l
	flex -oaddress_scanner.c address_scanner.l 

lib_LTLIBRARIES = libnw.la
libnw_la_SOURCES = $(NW_CORE)

nw_display_SOURCES = display.c canvas.c text_graph.c \
		simple_node_pos.c svg_graph.c readline.c \
		node_pos_alloc.c xml_utils.c graph_common.c \
		svg_graph_ortho.c svg_graph_radial.c

nw_display_LDADD = @LEXLIB@ -lm libnw.la

nw_clade_SOURCES = subtree.c
nw_clade_LDADD = @LEXLIB@ libnw.la

nw_reroot_SOURCES = reroot.c
nw_reroot_LDADD = @LEXLIB@ libnw.la

nw_rename_SOURCES = rename.c readline.c
nw_rename_LDADD = @LEXLIB@ libnw.la

nw_condense_SOURCES = condense.c
nw_condense_LDADD = @LEXLIB@ libnw.la

nw_support_SOURCES = support.c node_set.c
nw_support_LDADD = @LEXLIB@ libnw.la

nw_ed_SOURCES = address_scanner.l address_parser.y \
	tree_editor.c enode.c
nw_ed_LDADD = @LEXLIB@ -lm libnw.la

nw_topology_SOURCES = topology.c
nw_topology_LDADD = @LEXLIB@ libnw.la

nw_distance_SOURCES = distance.c simple_node_pos.c \
		      node_pos_alloc.c
nw_distance_LDADD = @LEXLIB@ libnw.la

nw_labels_SOURCES = labels.c 
nw_labels_LDADD = @LEXLIB@ libnw.la

nw_prune_SOURCES = prune.c 
nw_prune_LDADD = @LEXLIB@ libnw.la

nw_order_SOURCES = order.c order_tree.c
nw_order_LDADD = @LEXLIB@ libnw.la

nw_indent_SOURCES = indent.c indent_lex.l
nw_indent_LDADD = @LEXLIB@ libnw.la

nw_match_SOURCES = match.c order_tree.c
nw_match_LDADD = @LEXLIB@ libnw.la

nw_gen_SOURCES = generate.c tree_models.c
nw_gen_LDADD = libnw.la

nw_trim_SOURCES = trim.c
nw_trim_LDADD = @LEXLIB@ libnw.la

nw_duration_SOURCES = duration.c
nw_duration_LDADD = @LEXLIB@ libnw.la

nw_stats_SOURCES = stats.c
nw_stats_LDADD = @LEXLIB@ libnw.la

nw_toy_SOURCES = toy.c
nw_toy_LDADD = @LEXLIB@ libnw.la

# Removes automatically generated C code (Lex & YACC)
#
clean-local:
	$(RM) newick_scanner.c newick_parser.c newick_parser.h \
	address_scanner.c address_parser.c address_parser.h  \
	indent_lex.c
