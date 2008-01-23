AM_CFLAGS = -I ../src

TESTS = test_rnode test_redge test_list test_link \
	test_canvas test_concat test_hash test_lca \
	test_nodemap test_to_newick test_tree test_node_set \
	test_nw_reroot.sh test_nw_rename.sh test_nw_collapse.sh \
	test_nw_display.sh test_nw_indent.sh

check_PROGRAMS = test_rnode test_redge test_list test_link \
		 test_canvas test_concat test_hash test_lca \
		 test_nodemap test_to_newick test_tree test_node_set

SRC = $(top_builddir)/src

test_rnode_SOURCES = test_rnode.c $(SRC)/rnode.c $(SRC)/redge.c $(SRC)/list.c

test_redge_SOURCES = test_redge.c $(SRC)/rnode.c $(SRC)/redge.c $(SRC)/list.c

test_list_SOURCES = test_list.c $(SRC)/list.c

test_link_SOURCES = test_link.c $(SRC)/link.c $(SRC)/nodemap.c \
	$(SRC)/list.c $(SRC)/to_newick.c $(SRC)/redge.c $(SRC)/rnode.c \
	$(SRC)/concat.c tree_stubs.c 

test_canvas_SOURCES = test_canvas.c $(SRC)/canvas.c

test_concat_SOURCES = test_concat.c $(SRC)/concat.c

test_hash_SOURCES = test_hash.c $(SRC)/hash.c $(SRC)/list.c

test_lca_SOURCES = test_lca.c $(SRC)/lca.c $(SRC)/list.c $(SRC)/nodemap.c \
		    $(SRC)/link.c $(SRC)/rnode.c $(SRC)/redge.c tree_stubs.c

test_nodemap_SOURCES = test_nodemap.c $(SRC)/nodemap.c \
		$(SRC)/rnode.c $(SRC)/list.c

test_to_newick_SOURCES = test_to_newick.c $(SRC)/to_newick.c \
	$(SRC)/redge.c $(SRC)/rnode.c $(SRC)/link.c $(SRC)/concat.c \
	$(SRC)/list.c

test_tree_SOURCES = test_tree.c $(SRC)/tree.c $(SRC)/rnode.c $(SRC)/list.c \
	$(SRC)/to_newick.c $(SRC)/nodemap.c $(SRC)/link.c $(SRC)/concat.c \
	$(SRC)/redge.c tree_stubs.c

test_node_set_SOURCES = test_node_set.c tree_stubs.c $(SRC)/node_set.c \
	 $(SRC)/hash.c $(SRC)/rnode.c $(SRC)/list.c $(SRC)/link.c \
	 $(SRC)/redge.c
