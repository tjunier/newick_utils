
TESTS = check_rnode check_redge

check_PROGRAMS = check_rnode check_redge check_llist

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
