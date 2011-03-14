#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "tree.h"
#include "tree_stubs.h"
#include "nodemap.h"
#include "rnode_iterator.h"
#include "hash.h"
#include "rnode.h"
#include "list.h"
#include "parser.h"
#include "newick_parser.h"
#include "to_newick.h"

void newick_scanner_set_string_input(char *);

int test_iterator()
{
	const char *test_name = __func__;

	/* ((A:1,B:1.0)f:2.0,(C:1,(D:1,E:1)g:2)h:3)i; */
	struct rooted_tree tree = tree_3();
	struct hash *nodemap = create_label2node_map(tree.nodes_in_order);
	struct rnode *node_i = hash_get(nodemap, "i");
	struct rnode *node_f = hash_get(nodemap, "f");
	struct rnode *node_A = hash_get(nodemap, "A");
	struct rnode *node_B = hash_get(nodemap, "B");
	struct rnode *node_h = hash_get(nodemap, "h");
	struct rnode *node_C = hash_get(nodemap, "C");
	struct rnode *node_g = hash_get(nodemap, "g");
	struct rnode *node_D = hash_get(nodemap, "D");
	struct rnode *node_E = hash_get(nodemap, "E");

	struct rnode_iterator *it = create_rnode_iterator(tree.root);
	struct rnode *next;

	next = rnode_iterator_next(it);
	if (NULL == next) {
		printf ("%s: next node should not be NULL\n", test_name);
		return 1;
	}
	if (node_f != next) {
		printf ("%s: expected node f (%p), got %s (%p).\n",
				test_name, node_f, next->label, next);
		return 1;
	}
	next = rnode_iterator_next(it);
	if (NULL == next) {
		printf ("%s: next node should not be NULL\n", test_name);
		return 1;
	}
	if (node_A != next) {
		printf ("%s: expected node A (%p), got %s (%p).\n",
				test_name, node_A, next->label, next);
		return 1;
	}
	next = rnode_iterator_next(it);
	if (NULL == next) {
		printf ("%s: next node should not be NULL\n", test_name);
		return 1;
	}
	if (node_f != next) {
		printf ("%s: expected node f (%p), got %s (%p).\n",
				test_name, node_f, next->label, next);
		return 1;
	}
	next = rnode_iterator_next(it);
	if (NULL == next) {
		printf ("%s: next node should not be NULL\n", test_name);
		return 1;
	}
	if (node_B != next) {
		printf ("%s: expected node B (%p), got %s (%p).\n",
				test_name, node_B, next->label, next);
		return 1;
	}
	next = rnode_iterator_next(it);
	if (NULL == next) {
		printf ("%s: next node should not be NULL\n", test_name);
		return 1;
	}
	if (node_f != next) {
		printf ("%s: expected node f (%p), got %s (%p).\n",
				test_name, node_f, next->label, next);
		return 1;
	}
	next = rnode_iterator_next(it);
	if (NULL == next) {
		printf ("%s: next node should not be NULL\n", test_name);
		return 1;
	}
	if (node_i != next) {
		printf ("%s: expected node i (%p), got %s (%p).\n",
				test_name, node_i, next->label, next);
		return 1;
	}
	next = rnode_iterator_next(it);
	if (NULL == next) {
		printf ("%s: next node should not be NULL\n", test_name);
		return 1;
	}
	if (node_h != next) {
		printf ("%s: expected node h (%p), got %s (%p).\n",
				test_name, node_h, next->label, next);
		return 1;
	}
	next = rnode_iterator_next(it);
	if (NULL == next) {
		printf ("%s: next node should not be NULL\n", test_name);
		return 1;
	}
	if (node_C != next) {
		printf ("%s: expected node C (%p), got %s (%p).\n",
				test_name, node_C, next->label, next);
		return 1;
	}
	next = rnode_iterator_next(it);
	if (NULL == next) {
		printf ("%s: next node should not be NULL\n", test_name);
		return 1;
	}
	if (node_h != next) {
		printf ("%s: expected node h (%p), got %s (%p).\n",
				test_name, node_h, next->label, next);
		return 1;
	}
	next = rnode_iterator_next(it);
	if (NULL == next) {
		printf ("%s: next node should not be NULL\n", test_name);
		return 1;
	}
	if (node_g != next) {
		printf ("%s: expected node g (%p), got %s (%p).\n",
				test_name, node_g, next->label, next);
		return 1;
	}
	next = rnode_iterator_next(it);
	if (NULL == next) {
		printf ("%s: next node should not be NULL\n", test_name);
		return 1;
	}
	if (node_D != next) {
		printf ("%s: expected node D (%p), got %s (%p).\n",
				test_name, node_D, next->label, next);
		return 1;
	}
	next = rnode_iterator_next(it);
	if (NULL == next) {
		printf ("%s: next node should not be NULL\n", test_name);
		return 1;
	}
	if (node_g != next) {
		printf ("%s: expected node g (%p), got %s (%p).\n",
				test_name, node_g, next->label, next);
		return 1;
	}
	next = rnode_iterator_next(it);
	if (NULL == next) {
		printf ("%s: next node should not be NULL\n", test_name);
		return 1;
	}
	if (node_E != next) {
		printf ("%s: expected node E (%p), got %s (%p).\n",
				test_name, node_E, next->label, next);
		return 1;
	}
	next = rnode_iterator_next(it);
	if (NULL == next) {
		printf ("%s: next node should not be NULL\n", test_name);
		return 1;
	}
	if (node_g != next) {
		printf ("%s: expected node g (%p), got %s (%p).\n",
				test_name, node_g, next->label, next);
		return 1;
	}
	next = rnode_iterator_next(it);
	if (NULL == next) {
		printf ("%s: next node should not be NULL\n", test_name);
		return 1;
	}
	if (node_h != next) {
		printf ("%s: expected node h (%p), got %s (%p).\n",
				test_name, node_h, next->label, next);
		return 1;
	}
	next = rnode_iterator_next(it);
	if (NULL == next) {
		printf ("%s: next node should not be NULL\n", test_name);
		return 1;
	}
	if (node_i != next) {
		printf ("%s: expected node i (%p), got %s (%p).\n",
				test_name, node_i, next->label, next);
		return 1;
	}
	next = rnode_iterator_next(it);
	if (next != NULL) {
		printf ("%s: expected NULL, got %p.\n",
				test_name, next);
		return 1;
	}

	printf("%s ok.\n", test_name);
	return 0;
}

int test_iterator_midtree()
{
	const char *test_name = "test_iterator_midtree";

	/* ((A:1,B:1.0)f:2.0,(C:1,(D:1,E:1)g:2)h:3)i; */
	struct rooted_tree tree = tree_3();
	struct hash *nodemap = create_label2node_map(tree.nodes_in_order);
	struct rnode *node_h = hash_get(nodemap, "h");
	struct rnode *node_C = hash_get(nodemap, "C");
	struct rnode *node_g = hash_get(nodemap, "g");
	struct rnode *node_D = hash_get(nodemap, "D");
	struct rnode *node_E = hash_get(nodemap, "E");

	/* This time we start in the middle of the tree, at node h. */
	struct rnode_iterator *it = create_rnode_iterator(node_h);
	struct rnode *next;

	next = rnode_iterator_next(it);
	if (node_C != next) {
		printf ("%s: expected node C (%p), got %s (%p).\n",
				test_name, node_C, next->label, next);
		return 1;
	}
	next = rnode_iterator_next(it);
	if (node_h != next) {
		printf ("%s: expected node h (%p), got %s (%p).\n",
				test_name, node_h, next->label, next);
		return 1;
	}
	next = rnode_iterator_next(it);
	if (node_g != next) {
		printf ("%s: expected node g (%p), got %s (%p).\n",
				test_name, node_g, next->label, next);
		return 1;
	}
	next = rnode_iterator_next(it);
	if (node_D != next) {
		printf ("%s: expected node D (%p), got %s (%p).\n",
				test_name, node_D, next->label, next);
		return 1;
	}
	next = rnode_iterator_next(it);
	if (node_g != next) {
		printf ("%s: expected node g (%p), got %s (%p).\n",
				test_name, node_g, next->label, next);
		return 1;
	}
	next = rnode_iterator_next(it);
	if (node_E != next) {
		printf ("%s: expected node E (%p), got %s (%p).\n",
				test_name, node_E, next->label, next);
		return 1;
	}
	next = rnode_iterator_next(it);
	if (node_g != next) {
		printf ("%s: expected node g (%p), got %s (%p).\n",
				test_name, node_g, next->label, next);
		return 1;
	}
	next = rnode_iterator_next(it);
	if (node_h != next) {
		printf ("%s: expected node h (%p), got %s (%p).\n",
				test_name, node_h, next->label, next);
		return 1;
	}
	next = rnode_iterator_next(it);
	if (next != NULL) {
		printf ("%s: expected NULL, got %p.\n",
				test_name, next);
		return 1;
	}

	printf("%s ok.\n", test_name);
	return 0;
}

int test_iterator_leaf()
{
	const char *test_name = __func__;

	/* ((A:1,B:1.0)f:2.0,(C:1,(D:1,E:1)g:2)h:3)i; */
	struct rooted_tree tree = tree_3();
	struct hash *nodemap = create_label2node_map(tree.nodes_in_order);
	struct rnode *node_C = hash_get(nodemap, "C");
	struct rnode *node_A = hash_get(nodemap, "A");

	/* This time we start at a leaf (C) */
	struct rnode_iterator *it = create_rnode_iterator(node_C);
	struct rnode *next;

	next = rnode_iterator_next(it);
	if (node_C != next) {
		printf ("%s: expected node C (%p), got %s (%p).\n",
				test_name, node_C, next->label, next);
		return 1;
	}

	/* For good measure, we start again at another leaf */
	it = create_rnode_iterator(node_A);
	next = rnode_iterator_next(it);
	if (node_A != next) {
		printf ("%s: expected node A (%p), got %s (%p).\n",
				test_name, node_A, next->label, next);
		return 1;
	}

	printf("%s ok.\n", test_name);
	return 0;
}

int test_iterator_repeat()
{
	const char *test_name = __func__;

	/* ((A:1,B:1.0)f:2.0,(C:1,(D:1,E:1)g:2)h:3)i; */
	struct rooted_tree tree = tree_3();
	struct hash *nodemap = create_label2node_map(tree.nodes_in_order);
	struct rnode *node_h = hash_get(nodemap, "h");
	struct rnode *node_C = hash_get(nodemap, "C");
	struct rnode *node_g = hash_get(nodemap, "g");
	struct rnode *node_D = hash_get(nodemap, "D");
	struct rnode *node_E = hash_get(nodemap, "E");

	/* This time we start in the middle of the tree, at node h. */
	struct rnode_iterator *it = create_rnode_iterator(node_h);
	struct rnode *next;

	next = rnode_iterator_next(it);
	if (node_C != next) {
		printf ("%s: expected node C (%p), got %s (%p).\n",
				test_name, node_C, next->label, next);
		return 1;
	}
	next = rnode_iterator_next(it);
	if (node_h != next) {
		printf ("%s: expected node h (%p), got %s (%p).\n",
				test_name, node_h, next->label, next);
		return 1;
	}
	next = rnode_iterator_next(it);
	if (node_g != next) {
		printf ("%s: expected node g (%p), got %s (%p).\n",
				test_name, node_g, next->label, next);
		return 1;
	}
	next = rnode_iterator_next(it);
	if (node_D != next) {
		printf ("%s: expected node D (%p), got %s (%p).\n",
				test_name, node_D, next->label, next);
		return 1;
	}
	next = rnode_iterator_next(it);
	if (node_g != next) {
		printf ("%s: expected node g (%p), got %s (%p).\n",
				test_name, node_g, next->label, next);
		return 1;
	}
	next = rnode_iterator_next(it);
	if (node_E != next) {
		printf ("%s: expected node E (%p), got %s (%p).\n",
				test_name, node_E, next->label, next);
		return 1;
	}
	next = rnode_iterator_next(it);
	if (node_g != next) {
		printf ("%s: expected node g (%p), got %s (%p).\n",
				test_name, node_g, next->label, next);
		return 1;
	}
	next = rnode_iterator_next(it);
	if (node_h != next) {
		printf ("%s: expected node h (%p), got %s (%p).\n",
				test_name, node_h, next->label, next);
		return 1;
	}
	next = rnode_iterator_next(it);
	if (next != NULL) {
		printf ("%s: expected NULL, got %p.\n",
				test_name, next);
		return 1;
	}

	// Second pass
	it = create_rnode_iterator(node_h);

	next = rnode_iterator_next(it);
	if (node_C != next) {
		printf ("%s: expected node C (%p), got %s (%p).\n",
				test_name, node_C, next->label, next);
		return 1;
	}
	next = rnode_iterator_next(it);
	if (node_h != next) {
		printf ("%s: expected node h (%p), got %s (%p).\n",
				test_name, node_h, next->label, next);
		return 1;
	}
	next = rnode_iterator_next(it);
	if (node_g != next) {
		printf ("%s: expected node g (%p), got %s (%p).\n",
				test_name, node_g, next->label, next);
		return 1;
	}
	next = rnode_iterator_next(it);
	if (node_D != next) {
		printf ("%s: expected node D (%p), got %s (%p).\n",
				test_name, node_D, next->label, next);
		return 1;
	}
	next = rnode_iterator_next(it);
	if (node_g != next) {
		printf ("%s: expected node g (%p), got %s (%p).\n",
				test_name, node_g, next->label, next);
		return 1;
	}
	next = rnode_iterator_next(it);
	if (node_E != next) {
		printf ("%s: expected node E (%p), got %s (%p).\n",
				test_name, node_E, next->label, next);
		return 1;
	}
	next = rnode_iterator_next(it);
	if (node_g != next) {
		printf ("%s: expected node g (%p), got %s (%p).\n",
				test_name, node_g, next->label, next);
		return 1;
	}
	next = rnode_iterator_next(it);
	if (node_h != next) {
		printf ("%s: expected node h (%p), got %s (%p).\n",
				test_name, node_h, next->label, next);
		return 1;
	}
	next = rnode_iterator_next(it);
	if (next != NULL) {
		printf ("%s: expected NULL, got %p.\n",
				test_name, next);
		return 1;
	}


	printf("%s ok.\n", test_name);
	return 0;
}

int test_reset_current_child()
{
	const char *test_name = "test_current_child_elem";

	/* ((A:1,B:1.0)f:2.0,(C:1,(D:1,E:1)g:2)h:3)i; */
	struct rooted_tree tree = tree_3();
	struct hash *nodemap = create_label2node_map(tree.nodes_in_order);
	struct rnode *node_i = hash_get(nodemap, "i");
	struct rnode *node_f = hash_get(nodemap, "f");
	struct rnode *node_A = hash_get(nodemap, "A");
	struct rnode *node_B = hash_get(nodemap, "B");
	struct rnode *node_h = hash_get(nodemap, "h");
	struct rnode *node_C = hash_get(nodemap, "C");
	struct rnode *node_g = hash_get(nodemap, "g");
	struct rnode *node_D = hash_get(nodemap, "D");
	struct rnode *node_E = hash_get(nodemap, "E");

	/* Set the value to non-NULL (it is set to NULL on node creation) */
	node_i->current_child = (void *) 1;
	node_f->current_child = (void *) 1;
	node_A->current_child = (void *) 1;
	node_B->current_child = (void *) 1;
	node_h->current_child = (void *) 1;
	node_C->current_child = (void *) 1;
	node_g->current_child = (void *) 1;
	node_D->current_child = (void *) 1;
	node_E->current_child = (void *) 1;

	// Now, we reset:
	reset_current_child_elem(&tree);

	if (node_i->current_child != NULL) { 
		printf ("%s: reset_current_child_elem should"
			" have reset current_child_elem to NULL, "
			" got %p.\n", test_name, node_i->current_child);
		return 1;
	}
	if (node_f->current_child != NULL) { 
		printf ("%s: reset_current_child_elem should"
			" have reset current_child_elem to NULL, "
			" got %p.\n", test_name, node_f->current_child);
		return 1;
	}
	if (node_A->current_child != NULL) { 
		printf ("%s: reset_current_child should"
			" have reset current_child to NULL, "
			" got %p.\n", test_name, node_A->current_child);
		return 1;
	}
	if (node_B->current_child != NULL) { 
		printf ("%s: reset_current_child should"
			" have reset current_child to NULL, "
			" got %p.\n", test_name, node_B->current_child);
		return 1;
	}
	if (node_h->current_child != NULL) { 
		printf ("%s: reset_current_child should"
			" have reset current_child to NULL, "
			" got %p.\n", test_name, node_h->current_child);
		return 1;
	}
	if (node_C->current_child != NULL) { 
		printf ("%s: reset_current_child should"
			" have reset current_child to NULL, "
			" got %p.\n", test_name, node_C->current_child);
		return 1;
	}
	if (node_g->current_child != NULL) { 
		printf ("%s: reset_current_child should"
			" have reset current_child to NULL, "
			" got %p.\n", test_name, node_g->current_child);
		return 1;
	}
	if (node_D->current_child != NULL) { 
		printf ("%s: reset_current_child should"
			" have reset current_child to NULL, "
			" got %p.\n", test_name, node_D->current_child);
		return 1;
	}
	if (node_E->current_child != NULL) { 
		printf ("%s: reset_current_child should"
			" have reset current_child to NULL, "
			" got %p.\n", test_name, node_E->current_child);
		return 1;
	}

	printf("%s ok.\n", test_name);
	return 0;
}

int main()
{
	int failures = 0;
	printf("Starting rnode iterator test...\n");
	failures += test_iterator();
	failures += test_iterator_midtree();
	failures += test_iterator_leaf();
	failures += test_iterator_repeat();
	failures += test_reset_current_child();
	if (0 == failures) {
		printf("All tests ok.\n");
	} else {
		printf("%d test(s) FAILED.\n", failures);
		return 1;
	}

	return 0;
}
