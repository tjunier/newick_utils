AM_CFLAGS = -I ../src

TESTS = test_rnode test_redge test_list test_link \
	test_canvas test_concat test_hash test_lca test_enode \
	test_nodemap test_to_newick test_tree test_node_set \
	test_rnode_iterator test_tree_models test_xml_utils \
	test_nw_reroot.sh test_nw_rename.sh test_nw_condense.sh \
	test_nw_display.sh test_nw_indent.sh test_nw_support.sh \
	test_nw_ed.sh test_nw_topology.sh test_nw_clade.sh \
	test_nw_distance.sh test_nw_labels.sh test_nw_prune.sh \
	test_nw_order.sh test_nw_match.sh

check_PROGRAMS = test_rnode test_redge test_list test_link \
		 test_canvas test_concat test_hash test_lca \
		 test_nodemap test_to_newick test_tree test_node_set \
		 test_enode test_rnode_iterator test_readline \
		 test_tree_models test_xml_utils

check_HEADERS = tree_stubs.h

SRC = $(top_builddir)/src


test_rnode_SOURCES = test_rnode.c $(SRC)/rnode.c $(SRC)/redge.c $(SRC)/list.c \
	$(SRC)/rnode_iterator.c $(SRC)/hash.c

test_redge_SOURCES = test_redge.c $(SRC)/rnode.c $(SRC)/redge.c $(SRC)/list.c \
	$(SRC)/rnode_iterator.c $(SRC)/hash.c

test_list_SOURCES = test_list.c $(SRC)/list.c

test_link_SOURCES = test_link.c $(SRC)/link.c $(SRC)/nodemap.c \
	$(SRC)/list.c $(SRC)/to_newick.c $(SRC)/redge.c $(SRC)/rnode.c \
	$(SRC)/concat.c $(SRC)/hash.c tree_stubs.c \
	$(SRC)/rnode_iterator.c

test_canvas_SOURCES = test_canvas.c $(SRC)/canvas.c

test_concat_SOURCES = test_concat.c $(SRC)/concat.c

test_hash_SOURCES = test_hash.c $(SRC)/hash.c $(SRC)/list.c

test_lca_SOURCES = test_lca.c $(SRC)/lca.c $(SRC)/list.c $(SRC)/nodemap.c \
	$(SRC)/link.c $(SRC)/rnode.c $(SRC)/redge.c $(SRC)/hash.c \
	$(SRC)/rnode_iterator.c tree_stubs.c

test_nodemap_SOURCES = test_nodemap.c $(SRC)/nodemap.c \
	$(SRC)/rnode.c $(SRC)/list.c $(SRC)/hash.c \
	$(SRC)/rnode_iterator.c 

test_to_newick_SOURCES = test_to_newick.c $(SRC)/to_newick.c \
	$(SRC)/redge.c $(SRC)/rnode.c $(SRC)/link.c $(SRC)/concat.c \
	$(SRC)/list.c $(SRC)/rnode_iterator.c $(SRC)/hash.c

test_tree_SOURCES = test_tree.c $(SRC)/tree.c $(SRC)/rnode.c $(SRC)/list.c \
	$(SRC)/to_newick.c $(SRC)/nodemap.c $(SRC)/link.c $(SRC)/concat.c \
	$(SRC)/redge.c $(SRC)/hash.c tree_stubs.c $(SRC)/rnode_iterator.c 

test_node_set_SOURCES = test_node_set.c tree_stubs.c $(SRC)/node_set.c \
	$(SRC)/hash.c $(SRC)/rnode.c $(SRC)/list.c $(SRC)/link.c \
	$(SRC)/redge.c $(SRC)/rnode_iterator.c 

test_enode_SOURCES = test_enode.c $(SRC)/enode.c $(SRC)/rnode.c \
	$(SRC)/link.c $(SRC)/list.c $(SRC)/redge.c $(SRC)/rnode_iterator.c \
	$(SRC)/hash.c

test_rnode_iterator_SOURCES = test_rnode_iterator.c $(SRC)/rnode_iterator.c \
       	$(SRC)/list.c $(SRC)/link.c $(SRC)/rnode.c $(SRC)/redge.c \
       	$(SRC)/hash.c $(SRC)/nodemap.c tree_stubs.c	

test_readline_SOURCES = test_readline.c $(SRC)/readline.c

test_tree_models_SOURCES = test_tree_models.c $(SRC)/tree_models.c \
	$(SRC)/rnode.c $(SRC)/list.c $(SRC)/to_newick.c $(SRC)/link.c \
	$(SRC)/concat.c $(SRC)/redge.c $(SRC)/rnode_iterator.c \
	$(SRC)/hash.c

test_xml_utils_SOURCES = test_xml_utils.c $(SRC)/xml_utils.c

clean-local:
	$(RM) *.out
