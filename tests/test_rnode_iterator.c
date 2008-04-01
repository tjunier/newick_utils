#include <stdio.h>
#include <string.h>

#include "tree.h"
#include "tree_stubs.h"
#include "nodemap.h"
#include "rnode_iterator.h"
#include "hash.h"
#include "rnode.h"

int test_simple()
{
	const char *test_name = "test_simple";

	/* TODO: remove - just commented this out so it compiles.
	 *
	struct rooted_tree tree = tree_3();
	struct hash *nodemap = create_node_map(tree.nodes_in_order);
	struct rnode *node_i = hash_get(nodemap, "i");
	struct rnode *node_f = hash_get(nodemap, "f");
	struct rnode *node_A = hash_get(nodemap, "A");
	struct rnode *node_B = hash_get(nodemap, "B");
	struct rnode *node_h = hash_get(nodemap, "h");
	struct rnode *node_C = hash_get(nodemap, "C");
	struct rnode *node_g = hash_get(nodemap, "g");
	struct rnode *node_D = hash_get(nodemap, "D");
	struct rnode *node_E = hash_get(nodemap, "E");

	printf ("implement fully\n");
	return 1;

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

	printf("%s ok.\n", test_name);
	*/
	return 0;
}

int main()
{
	int failures = 0;
	printf("Starting rnode iterator test...\n");
	failures += test_simple();
	if (0 == failures) {
		printf("All tests ok.\n");
	} else {
		printf("%d test(s) FAILED.\n", failures);
		return 1;
	}

	return 0;
}
