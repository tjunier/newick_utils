# There is no real need for ../../src, but 'make distcheck' won't work without
# it. Or should this be done with VPATH?
AM_CFLAGS = -I../src -I../../src

SRCDIR = @srcdir@
export SRCDIR

showsrc:
	@echo $(srcdir)

TESTS = test_newick_scanner test_newick_parser test_rnode test_list \
	test_link test_masprintf test_svg_graph_radial \
	test_canvas test_concat test_hash test_lca test_enode \
	test_nodemap test_to_newick test_tree test_node_set \
	test_rnode_iterator test_tree_models test_xml_utils \
	test_error test_order_tree test_graph_common \
	test_subtree \
	test_nw_reroot.sh test_nw_rename.sh test_nw_condense.sh \
	test_nw_display.sh test_nw_indent.sh test_nw_support.sh \
	test_nw_ed.sh test_nw_topology.sh test_nw_clade.sh \
	test_nw_distance.sh test_nw_labels.sh test_nw_prune.sh \
	test_nw_order.sh test_nw_match.sh test_nw_trim.sh \
	test_nw_gen.sh test_nw_duration.sh test_nw_stats.sh \
	test_nw_sched.sh test_nw_luaed.sh \
	test_summary.sh	# keep this one at the end!

check_PROGRAMS = test_rnode test_list test_link test_newick_scanner \
		 test_canvas test_concat test_hash test_lca \
		 test_nodemap test_to_newick test_tree test_node_set \
		 test_enode test_rnode_iterator test_readline \
		 test_tree_models test_xml_utils test_masprintf \
		 test_error test_order_tree test_graph_common \
		 test_newick_parser test_svg_graph_radial \
		 test_subtree

check_HEADERS = tree_stubs.h $(SRC)/rnode.h

SRC = $(top_builddir)/src

test_newick_scanner_SOURCES = test_newick_scanner.c $(SRC)/newick_scanner.c \
	$(SRC)/newick_parser.c $(SRC)/rnode.c $(SRC)/rnode_iterator.c \
	$(SRC)/list.c $(SRC)/hash.c $(SRC)/masprintf.c $(SRC)/link.c

test_newick_parser_SOURCES = test_newick_parser.c $(SRC)/parser.c \
	$(SRC)/newick_scanner.c $(SRC)/newick_parser.c $(SRC)/list.c \
	$(SRC)/rnode.c $(SRC)/link.c $(SRC)/hash.c $(SRC)/rnode_iterator.c \
	$(SRC)/masprintf.c $(SRC)/to_newick.c $(SRC)/concat.c

test_rnode_SOURCES = test_rnode.c $(SRC)/rnode.c $(SRC)/list.c \
	$(SRC)/rnode_iterator.c $(SRC)/hash.c $(SRC)/masprintf.c \
	tree_stubs.c $(SRC)/nodemap.c $(SRC)/link.c

test_list_SOURCES = test_list.c $(SRC)/list.c

test_link_SOURCES = test_link.c $(SRC)/link.c $(SRC)/nodemap.c \
	$(SRC)/list.c $(SRC)/to_newick.c $(SRC)/rnode.c \
	$(SRC)/concat.c $(SRC)/hash.c tree_stubs.c \
	$(SRC)/rnode_iterator.c $(SRC)/masprintf.c

test_canvas_SOURCES = test_canvas.c $(SRC)/canvas.c

test_concat_SOURCES = test_concat.c $(SRC)/concat.c

test_hash_SOURCES = test_hash.c $(SRC)/hash.c $(SRC)/list.c $(SRC)/masprintf.c

test_lca_SOURCES = test_lca.c $(SRC)/lca.c $(SRC)/list.c $(SRC)/nodemap.c \
	$(SRC)/link.c $(SRC)/rnode.c $(SRC)/hash.c \
	$(SRC)/rnode_iterator.c tree_stubs.c $(SRC)/masprintf.c \
	$(SRC)/error.c

test_nodemap_SOURCES = test_nodemap.c $(SRC)/nodemap.c \
	$(SRC)/rnode.c $(SRC)/list.c $(SRC)/hash.c $(SRC)/link.c \
	$(SRC)/rnode_iterator.c $(SRC)/masprintf.c tree_stubs.c

test_to_newick_SOURCES = test_to_newick.c $(SRC)/to_newick.c \
	$(SRC)/rnode.c $(SRC)/link.c $(SRC)/concat.c \
	$(SRC)/list.c $(SRC)/rnode_iterator.c $(SRC)/hash.c \
	$(SRC)/masprintf.c $(SRC)/parser.c $(SRC)/newick_scanner.c \
	$(SRC)/newick_parser.c tree_stubs.c

test_tree_SOURCES = test_tree.c $(SRC)/tree.c $(SRC)/rnode.c $(SRC)/list.c \
	$(SRC)/to_newick.c $(SRC)/nodemap.c $(SRC)/link.c $(SRC)/concat.c \
	$(SRC)/hash.c tree_stubs.c $(SRC)/rnode_iterator.c \
	$(SRC)/masprintf.c

test_node_set_SOURCES = test_node_set.c tree_stubs.c $(SRC)/node_set.c \
	$(SRC)/hash.c $(SRC)/rnode.c $(SRC)/list.c $(SRC)/link.c \
	$(SRC)/rnode_iterator.c $(SRC)/masprintf.c

test_enode_SOURCES = test_enode.c $(SRC)/enode.c $(SRC)/rnode.c \
	$(SRC)/link.c $(SRC)/list.c $(SRC)/rnode_iterator.c \
	$(SRC)/hash.c $(SRC)/masprintf.c

test_rnode_iterator_SOURCES = test_rnode_iterator.c $(SRC)/rnode_iterator.c \
  	$(SRC)/list.c $(SRC)/link.c $(SRC)/rnode.c $(SRC)/to_newick.c \
       	$(SRC)/hash.c $(SRC)/nodemap.c tree_stubs.c $(SRC)/masprintf.c \
	$(SRC)/parser.c $(SRC)/newick_scanner.c $(SRC)/newick_parser.c \
	$(SRC)/concat.c

test_readline_SOURCES = test_readline.c $(SRC)/readline.c

test_tree_models_SOURCES = test_tree_models.c $(SRC)/tree_models.c \
	$(SRC)/rnode.c $(SRC)/list.c $(SRC)/to_newick.c $(SRC)/link.c \
	$(SRC)/concat.c $(SRC)/rnode_iterator.c \
	$(SRC)/hash.c $(SRC)/masprintf.c

test_xml_utils_SOURCES = test_xml_utils.c $(SRC)/xml_utils.c \
	$(SRC)/masprintf.c

test_masprintf_SOURCES = test_masprintf.c $(SRC)/masprintf.c

test_error_SOURCES = test_error.c $(SRC)/error.c

test_order_tree_SOURCES = test_order_tree.c $(SRC)/order_tree.c tree_stubs.c \
	$(SRC)/link.c $(SRC)/to_newick.c $(SRC)/rnode.c $(SRC)/list.c \
	$(SRC)/masprintf.c $(SRC)/concat.c $(SRC)/hash.c $(SRC)/nodemap.c \
	$(SRC)/rnode_iterator.c

test_graph_common_SOURCES = test_graph_common.c $(SRC)/graph_common.c \
	tree_stubs.c $(SRC)/link.c $(SRC)/list.c $(SRC)/tree.c \
	$(SRC)/rnode_iterator.c $(SRC)/hash.c $(SRC)/masprintf.c \
	$(SRC)/rnode.c $(SRC)/nodemap.c

test_svg_graph_radial_SOURCES = test_svg_graph_radial.c \
	$(SRC)/svg_graph_radial.c $(SRC)/tree.c $(SRC)/svg_graph.c \
	$(SRC)/rnode.c $(SRC)/hash.c $(SRC)/list.c $(SRC)/masprintf.c \
	$(SRC)/rnode_iterator.c $(SRC)/svg_graph_ortho.c $(SRC)/error.c \
	$(SRC)/readline.c $(SRC)/xml_utils.c $(SRC)/graph_common.c \
	$(SRC)/node_pos_alloc.c $(SRC)/nodemap.c $(SRC)/lca.c $(SRC)/link.c

test_subtree_SOURCES = test_subtree.c $(SRC)/subtree.c $(SRC)/rnode.c \
	$(SRC)/list.c $(SRC)/hash.c $(SRC)/link.c $(SRC)/rnode_iterator.c \
	$(SRC)/masprintf.c $(SRC)/nodemap.c

clean-local:
	$(RM) *.out
