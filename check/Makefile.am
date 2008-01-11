
TESTS = check_rnode check_redge check_llist check_link \
	check_canvas check_concat check_hash check_lca \
	check_nodemap check_to_newick check_tree

check_PROGRAMS = check_rnode check_redge check_llist check_link \
		 check_canvas check_concat check_hash check_lca \
		 check_nodemap check_to_newick check_tree

SRC = $(top_builddir)/src

check_rnode_SOURCES = check_rnode.c $(SRC)/rnode.c $(SRC)/redge.c $(SRC)/list.c
check_rnode_CFLAGS = @CHECK_CFLAGS@
check_rnode_LDADD = @CHECK_LIBS@

check_redge_SOURCES = check_redge.c $(SRC)/rnode.c $(SRC)/redge.c $(SRC)/list.c
check_redge_CFLAGS = @CHECK_CFLAGS@
check_redge_LDADD = @CHECK_LIBS@

check_llist_SOURCES = check_llist.c $(SRC)/list.c
check_llist_CFLAGS = @CHECK_CFLAGS@
check_llist_LDADD = @CHECK_LIBS@

check_link_SOURCES = check_link.c $(SRC)/link.c $(SRC)/nodemap.c \
	$(SRC)/list.c $(SRC)/to_newick.c $(SRC)/redge.c $(SRC)/rnode.c \
	$(SRC)/concat.c tree_stubs.c 
check_link_CFLAGS = @CHECK_CFLAGS@
check_link_LDADD = @CHECK_LIBS@

check_canvas_SOURCES = check_canvas.c $(SRC)/canvas.c
check_canvas_CFLAGS = @CHECK_CFLAGS@
check_canvas_LDADD = @CHECK_LIBS@

check_concat_SOURCES = check_concat.c $(SRC)/concat.c
check_concat_CFLAGS = @CHECK_CFLAGS@
check_concat_LDADD = @CHECK_LIBS@

check_hash_SOURCES = check_hash.c $(SRC)/hash.c $(SRC)/list.c
check_hash_CFLAGS = @CHECK_CFLAGS@
check_hash_LDADD = @CHECK_LIBS@

check_lca_SOURCES = check_lca.c $(SRC)/lca.c $(SRC)/list.c $(SRC)/nodemap.c \
		    $(SRC)/link.c $(SRC)/rnode.c $(SRC)/redge.c tree_stubs.c
check_lca_CFLAGS = @CHECK_CFLAGS@
check_lca_LDADD = @CHECK_LIBS@

check_nodemap_SOURCES = check_nodemap.c $(SRC)/nodemap.c \
		$(SRC)/rnode.c $(SRC)/list.c
check_nodemap_CFLAGS = @CHECK_CFLAGS@
check_nodemap_LDADD = @CHECK_LIBS@

check_to_newick_SOURCES = check_to_newick.c $(SRC)/to_newick.c \
	$(SRC)/redge.c $(SRC)/rnode.c $(SRC)/link.c $(SRC)/concat.c \
	$(SRC)/list.c
check_to_newick_CFLAGS = @CHECK_CFLAGS@
check_to_newick_LDADD = @CHECK_LIBS@

check_tree_SOURCES = check_tree.c $(SRC)/tree.c $(SRC)/rnode.c $(SRC)/list.c \
	$(SRC)/to_newick.c $(SRC)/nodemap.c $(SRC)/link.c $(SRC)/concat.c \
	$(SRC)/redge.c tree_stubs.c
check_tree_CFLAGS = @CHECK_CFLAGS@
check_tree_LDADD = @CHECK_LIBS@

