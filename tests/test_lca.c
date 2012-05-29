#include <stdio.h>
#include <string.h>

#include "lca.h"
#include "tree_stubs.h"
#include "to_newick.h"
#include "rnode.h"
#include "nodemap.h"
#include "list.h"
#include "tree.h"
#include "hash.h"

int test_lca2()
{
	const char *test_name = "test_lca2";

	struct rooted_tree tree;
	struct rnode *lca, *desc_A, *desc_B, *desc_C, *desc_D, *desc_E;
	struct rnode *desc_f, *desc_g, *desc_h, *desc_i;
	struct hash *map;

	tree = tree_2();	/* ((A,B)f,(C,(D,E)g)h)i; - see tree_stubs.h */
	map = create_label2node_map(tree.nodes_in_order);
	desc_A = hash_get(map, "A");
	desc_B = hash_get(map, "B");
	desc_C = hash_get(map, "C");
	desc_D = hash_get(map, "D");
	desc_E = hash_get(map, "E");
	desc_f = hash_get(map, "f");
	desc_g = hash_get(map, "g");
	desc_h = hash_get(map, "h");
	desc_i = hash_get(map, "i");

	lca = lca2(&tree, desc_A, desc_B);
	if (desc_f != lca) {
		printf ("%s: expected node 'f' as LCA of 'A' and 'B' (got '%s').\n",
				test_name, lca->label);
		return 1;
	}
	lca = lca2(&tree, desc_D, desc_E);
	if (desc_g != lca) {
		printf ("%s: expected node 'g' as LCA of 'D' and 'E' (got '%s').\n",
				test_name, lca->label);
		return 1;
	}
	lca = lca2(&tree, desc_A, desc_A);
	if (desc_A != lca) {
		printf ("%s: expected node 'A' as LCA of 'A' and 'A' (got '%s').\n",
				test_name, lca->label);
		return 1;
	}
	lca = lca2(&tree, desc_A, desc_C);
	if (desc_i != lca) {
		printf ("%s: expected node 'i' as LCA of 'A' and 'C' (got '%s').\n",
				test_name, lca->label);
		return 1;
	}
	lca = lca2(&tree, desc_C, desc_A);
	if (desc_i != lca) {
		printf ("%s: expected node 'i' as LCA of 'C' and 'A' (got '%s').\n",
				test_name, lca->label);
		return 1;
	}
	lca = lca2(&tree, desc_h, desc_f);
	if (desc_i != lca) {
		printf ("%s: expected node 'i' as LCA of 'h' and 'f' (got '%s').\n",
				test_name, lca->label);
		return 1;
	}
	lca = lca2(&tree, desc_h, desc_E);
	if (desc_h != lca) {
		printf ("%s: expected node 'h' as LCA of 'h' and 'E' (got '%s').\n",
				test_name, lca->label);
		return 1;
	}
	
	printf("%s ok.\n", test_name);
	return 0;
}

int test_lca_from_labels()
{
	const char *test_name = "test_lca_from_labels";

	struct rooted_tree tree;
	struct rnode *ancestor, *desc_A, *desc_B;
	struct rnode *desc_C, *desc_D, *desc_E;
	struct rnode *desc_f, *desc_g, *desc_h, *desc_i;
	struct hash *map;
	struct llist *desc_labels;

	tree = tree_2();	/* ((A,B)f,(C,(D,E)g)h)i; - see tree_stubs.h */
	map = create_label2node_map(tree.nodes_in_order);
	desc_A = hash_get(map, "A");
	desc_B = hash_get(map, "B");
	desc_C = hash_get(map, "C");
	desc_D = hash_get(map, "D");
	desc_E = hash_get(map, "E");
	desc_f = hash_get(map, "f");
	desc_g = hash_get(map, "g");
	desc_h = hash_get(map, "h");
	desc_i = hash_get(map, "i");

	desc_labels = create_llist();
	append_element(desc_labels, "A");
	append_element(desc_labels, "B");

	ancestor = lca_from_labels(&tree, desc_labels);
	if (desc_f != ancestor) {
		printf ("%s: expected node 'f' as LCA of 'A' and 'B' (got '%s').\n",
				test_name, ancestor->label);
		return 1;
	}

	desc_labels = create_llist();
	append_element(desc_labels, "C");
	append_element(desc_labels, "D");
	append_element(desc_labels, "E");
	ancestor = lca_from_labels(&tree, desc_labels);
	if (desc_h != ancestor) {
		printf ("%s: expected node 'h' as LCA of 'C', 'D' and 'E' (got '%s').\n",
				test_name, ancestor->label);
		return 1;
	}

	desc_labels = create_llist();
	append_element(desc_labels, "C");
	append_element(desc_labels, "g");
	ancestor = lca_from_labels(&tree, desc_labels);
	if (desc_h != ancestor) {
		printf ("%s: expected node 'h' as LCA of 'C', 'D' and 'E' (got '%s').\n",
				test_name, ancestor->label);
		return 1;
	}

	printf("%s ok.\n", test_name);
	return 0;
}

int test_lca_from_nodes()
{
	const char *test_name = "test_lca_from_nodes";

	struct rooted_tree tree;
	struct rnode *ancestor, *desc_A, *desc_B;
	struct rnode *desc_C, *desc_D, *desc_E;
	struct rnode *desc_f, *desc_g, *desc_h, *desc_i;
	struct hash *map;
	struct llist *descendants;

	tree = tree_2();	/* ((A,B)f,(C,(D,E)g)h)i; - see tree_stubs.h */
	map = create_label2node_map(tree.nodes_in_order);
	desc_A = hash_get(map, "A");
	desc_B = hash_get(map, "B");
	desc_C = hash_get(map, "C");
	desc_D = hash_get(map, "D");
	desc_E = hash_get(map, "E");
	desc_f = hash_get(map, "f");
	desc_g = hash_get(map, "g");
	desc_h = hash_get(map, "h");
	desc_i = hash_get(map, "i");

	descendants = create_llist();
	append_element(descendants, desc_A);
	append_element(descendants, desc_B);
	ancestor = lca_from_nodes(&tree, descendants);

	if (desc_f != ancestor) {
		printf ("%s: expected node 'f' as LCA of 'A' and 'B' (got '%s').\n",
				test_name, ancestor->label);
		return 1;
	}
	if (descendants->count != 2) {
		printf ("%s: argument list was modified.\n",
				test_name);
		return 1;
	}

	descendants = create_llist();
	append_element(descendants, desc_C);
	append_element(descendants, desc_D);
	append_element(descendants, desc_E);
	ancestor = lca_from_nodes(&tree, descendants);

	if (desc_h != ancestor) {
		printf ("%s: expected node 'h' as LCA of 'C', 'D' and 'E' (got '%s').\n",
				test_name, ancestor->label);
		return 1;
	}
	if (descendants->count != 3) {
		printf ("%s: argument list was modified.\n",
				test_name);
		return 1;
	}

	descendants = create_llist();
	append_element(descendants, desc_C);
	append_element(descendants, desc_g);
	ancestor = lca_from_nodes(&tree, descendants);

	if (desc_h != ancestor) {
		printf ("%s: expected node 'h' as LCA of 'C', 'D' and 'E' (got '%s').\n",
				test_name, ancestor->label);
		return 1;
	}
	if (descendants->count != 2) {
		printf ("%s: argument list was modified.\n",
				test_name);
		return 1;
	}

	printf("%s ok.\n", test_name);
	return 0;
}

int test_lca_from_labels_multi()
{
	const char *test_name = "test_lca_from_labels_multi";

	/* (((D,D)e,D)f,((C,B)g,(B,A)h)i)j; */
	struct rooted_tree tree = tree_9();
	struct rnode *lca;
	struct llist *labels = create_llist();

	append_element(labels, "C");
	lca = lca_from_labels_multi(&tree, labels);
	if (strcmp(lca->label, "C") != 0) {
		printf ("%s: expected 'C', got '%s'\n", test_name,
			lca->label);
		return 1;
	}

	clear_llist(labels);
	append_element(labels, "D");
	lca = lca_from_labels_multi(&tree, labels);
	if (strcmp(lca->label, "f") != 0) {
		printf ("%s: expected 'f', got '%s'\n", test_name,
			lca->label);
		return 1;
	}

	clear_llist(labels);
	append_element(labels, "B");
	lca = lca_from_labels_multi(&tree, labels);
	if (strcmp(lca->label, "i") != 0) {
		printf ("%s: expected 'i', got '%s'\n", test_name,
			lca->label);
		return 1;
	}

	append_element(labels, "D"); /* now contains B and D */
	lca = lca_from_labels_multi(&tree, labels);
	if (strcmp(lca->label, "j") != 0) {
		printf ("%s: expected 'j', got '%s'\n", test_name,
			lca->label);
		return 1;
	}

	printf("%s ok.\n", test_name);
	return 0;
}

int main()
{
	int failures = 0;
	printf("Starting lca test...\n");
	failures += test_lca2();
	failures += test_lca_from_labels();
	failures += test_lca_from_labels_multi();
	failures += test_lca_from_nodes();
	if (0 == failures) {
		printf("All tests ok.\n");
	} else {
		printf("%d test(s) FAILED.\n", failures);
		return 1;
	}

	return 0;
}
