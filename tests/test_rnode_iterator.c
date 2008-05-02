#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "tree.h"
#include "tree_stubs.h"
#include "nodemap.h"
#include "rnode_iterator.h"
#include "hash.h"
#include "rnode.h"
#include "list.h"

int test_iterator()
{
	const char *test_name = "test_iterator";

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
	if (node_f != next) {
		printf ("%s: expected node f (%p), got %s (%p).\n",
				test_name, node_f, next->label, next);
		return 1;
	}
	next = rnode_iterator_next(it);
	if (node_A != next) {
		printf ("%s: expected node A (%p), got %s (%p).\n",
				test_name, node_A, next->label, next);
		return 1;
	}
	next = rnode_iterator_next(it);
	if (node_f != next) {
		printf ("%s: expected node f (%p), got %s (%p).\n",
				test_name, node_f, next->label, next);
		return 1;
	}
	next = rnode_iterator_next(it);
	if (node_B != next) {
		printf ("%s: expected node B (%p), got %s (%p).\n",
				test_name, node_B, next->label, next);
		return 1;
	}
	next = rnode_iterator_next(it);
	if (node_f != next) {
		printf ("%s: expected node f (%p), got %s (%p).\n",
				test_name, node_f, next->label, next);
		return 1;
	}
	next = rnode_iterator_next(it);
	if (node_i != next) {
		printf ("%s: expected node i (%p), got %s (%p).\n",
				test_name, node_i, next->label, next);
		return 1;
	}
	next = rnode_iterator_next(it);
	if (node_h != next) {
		printf ("%s: expected node h (%p), got %s (%p).\n",
				test_name, node_h, next->label, next);
		return 1;
	}
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
	struct rnode *node_i = hash_get(nodemap, "i");
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

int test_get_nodes_in_order()
{
	const char *test_name = "test_get_nodes_in_order";

	/* (A:3,B:3,(C:2,(D:1,E:1)f:1)g:1)h; */
	struct rooted_tree tree = tree_5();
	/* What is being tested here is the get_nodes_in_order() function,
	 * which computes the ordered nodes list from a root node (the usual
	 * way is to compute it while parsing Newick) */
	struct llist *nodes_in_order = get_nodes_in_order(tree.root);

	struct list_elem *elem = nodes_in_order->head;
	struct rnode *node = (struct rnode *)elem->data;
	if (strcmp("A", node->label)) {
		printf ("%s: expected node A, got %s.\n", test_name, node->label);
		return 1;
	}
	elem = elem->next;
	node = (struct rnode *) elem->data;
	if (strcmp("B", node->label)) {
		printf ("%s: expected node B, got %s.\n", test_name, node->label);
		return 1;
	}
	elem = elem->next;
	node = (struct rnode *) elem->data;
	if (strcmp("C", node->label)) {
		printf ("%s: expected node C, got %s.\n", test_name, node->label);
		return 1;
	}
	elem = elem->next;
	node = (struct rnode *) elem->data;
	if (strcmp("D", node->label)) {
		printf ("%s: expected node D, got %s.\n", test_name, node->label);
		return 1;
	}
	elem = elem->next;
	node = (struct rnode *) elem->data;
	if (strcmp("E", node->label)) {
		printf ("%s: expected node E, got %s.\n", test_name, node->label);
		return 1;
	}
	elem = elem->next;
	node = (struct rnode *) elem->data;
	if (strcmp("f", node->label)) {
		printf ("%s: expected node f, got %s.\n", test_name, node->label);
		return 1;
	}
	elem = elem->next;
	node = (struct rnode *) elem->data;
	if (strcmp("g", node->label)) {
		printf ("%s: expected node g, got %s.\n", test_name, node->label);
		return 1;
	}
	elem = elem->next;
	node = (struct rnode *) elem->data;
	if (strcmp("h", node->label)) {
		printf ("%s: expected node h, got %s.\n", test_name, node->label);
		return 1;
	}

	printf("%s ok.\n", test_name);
	return 0;
}

int test_get_leaf_label_map()
{

	const char *test_name = "test_get_leaf_label_map";

	/* ((A:1,B:1.0)f:2.0,(C:1,(D:1,E:1)g:2)h:3)i; */
	struct rooted_tree tree = tree_3();
	struct hash *map = get_leaf_label_map(tree.root);

	if (5 != map->count) {
		printf ("%s: expected hash count of 5, got %d.\n", test_name, map->count);
		return 1;
	}
	if (NULL == hash_get(map, "A")) {
		printf ("%s: leaf A not found in map.\n", test_name);
		return 1;
	}
	if (NULL == hash_get(map, "B")) {
		printf ("%s: leaf B not found in map.\n", test_name);
		return 1;
	}
	if (NULL == hash_get(map, "C")) {
		printf ("%s: leaf C not found in map.\n", test_name);
		return 1;
	}
	if (NULL == hash_get(map, "D")) {
		printf ("%s: leaf D not found in map.\n", test_name);
		return 1;
	}
	if (NULL == hash_get(map, "E")) {
		printf ("%s: leaf E not found in map.\n", test_name);
		return 1;
	}

	printf ("%s ok.\n", test_name);
	return 0;
}

int main()
{
	int failures = 0;
	printf("Starting rnode iterator test...\n");
	failures += test_iterator();
	failures += test_iterator_midtree();
	failures += test_get_leaf_label_map();
	// failures += test_get_nodes_in_order();
	if (0 == failures) {
		printf("All tests ok.\n");
	} else {
		printf("%d test(s) FAILED.\n", failures);
		return 1;
	}

	return 0;
}
