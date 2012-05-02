bin_PROGRAMS = nw_indent nw_display nw_clade nw_reroot nw_rename \
	       nw_condense nw_support nw_ed nw_topology nw_distance \
	       nw_labels nw_prune nw_order nw_match nw_gen nw_trim \
	       nw_duration nw_stats

if WANT_NW_SCHED
bin_PROGRAMS += nw_sched
endif

if WANT_NW_LUAED
bin_PROGRAMS += nw_luaed
endif

# I mention all headers here, so they're included in the distro (otherwise this
# is not necessary, it builds just fine)
HDR = canvas.h concat.h enode.h error.h hash.h lca.h link.h list.h nodemap.h \
	node_pos_alloc.h node_set.h parser.h readline.h rnode.h \
	rnode_iterator.h simple_node_pos.h svg_graph.h text_graph.h \
	to_newick.h tree.h tree_editor_rnode_data.h common.h order_tree.h \
	tree_models.h xml_utils.h graph_common.h svg_graph_common.h \
	svg_graph_radial.h svg_graph_ortho.h masprintf.h subtree.h \
	newick_parser.h

NW_CORE = newick_parser.c newick_scanner.c rnode.c list.c parser.c \
	link.c tree.c nodemap.c hash.c rnode_iterator.c \
	masprintf.c to_newick.c concat.c lca.c error.c $(HDR)

newick_scanner.c: newick_scanner.l
	flex -o newick_scanner.c newick_scanner.l

newick_parser.c newick_parser.h: newick_parser.y
	bison -d -o newick_parser.c -p nws newick_parser.y

indent_lex.c: indent_lex.l
	flex -o indent_lex.c indent_lex.l

address_parser.c address_parser.h: address_parser.y
	bison -d -o address_parser.c address_parser.y

address_scanner.c: address_scanner.l address_parser.h
	flex -o address_scanner.c address_scanner.l

lib_LTLIBRARIES = libnw.la
libnw_la_SOURCES = $(NW_CORE)

nw_display_SOURCES = display.c canvas.c text_graph.c \
		simple_node_pos.c svg_graph.c readline.c \
		node_pos_alloc.c xml_utils.c graph_common.c \
		svg_graph_ortho.c svg_graph_radial.c 
nw_display_LDADD = -lm libnw.la

nw_clade_SOURCES = clade.c subtree.c
nw_clade_LDADD = libnw.la

nw_reroot_SOURCES = reroot.c
nw_reroot_LDADD = libnw.la

nw_rename_SOURCES = rename.c readline.c
nw_rename_LDADD = libnw.la

nw_condense_SOURCES = condense.c
nw_condense_LDADD = libnw.la

nw_support_SOURCES = support.c node_set.c
nw_support_LDADD = libnw.la

nw_ed_SOURCES = address_scanner.c address_parser.c address_parser.h \
		tree_editor.c enode.c address_parser_status.h
nw_ed_LDADD = -lm libnw.la

nw_topology_SOURCES = topology.c
nw_topology_LDADD = libnw.la

nw_distance_SOURCES = distance.c simple_node_pos.c \
		      node_pos_alloc.c
nw_distance_LDADD = libnw.la

nw_labels_SOURCES = labels.c 
nw_labels_LDADD = libnw.la

nw_prune_SOURCES = prune.c 
nw_prune_LDADD = libnw.la

nw_order_SOURCES = order.c order_tree.c
nw_order_LDADD = libnw.la

nw_indent_SOURCES = indent.c indent_lex.c

nw_match_SOURCES = match.c order_tree.c
nw_match_LDADD = libnw.la

nw_gen_SOURCES = generate.c tree_models.c
nw_gen_LDADD = libnw.la

nw_trim_SOURCES = trim.c
nw_trim_LDADD = libnw.la

nw_duration_SOURCES = duration.c
nw_duration_LDADD = libnw.la

nw_stats_SOURCES = stats.c
nw_stats_LDADD = libnw.la

nw_sched_SOURCES = scheme_tree_editor.c rnode_smob.c rnode_smob.h
nw_sched_LDADD = libnw.la

nw_luaed_SOURCES = lua_tree_editor.c 
nw_luaed_LDADD = libnw.la

# Removes automatically generated C code (Lex & YACC)
#
clean-local:
	$(RM) newick_scanner.c newick_parser.c newick_parser.h \
	address_scanner.c address_parser.c address_parser.h  \
	indent_lex.c
