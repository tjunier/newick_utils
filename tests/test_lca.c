#include <stdio.h>
#include <string.h>

#include "lca.h"
#include "tree_stubs.h"
#include "to_newick.h"
#include "rnode.h"
#include "nodemap.h"
#include "list.h"
#include "tree.h"

int test_lca2()
{
	const char *test_name = "test_lca2";

	struct rooted_tree tree;
	struct rnode *lca, *desc_A, *desc_B, *desc_C, *desc_D, *desc_E;
	struct rnode *desc_f, *desc_g, *desc_h, *desc_i;
	struct node_map *map;

	tree = tree_2();	/* ((A,B)f,(C,(D,E)g)h)i; - see tree_stubs.h */
	map = create_node_map(tree.nodes_in_order);
	desc_A = get_node_with_label(map, "A");
	desc_B = get_node_with_label(map, "B");
	desc_C = get_node_with_label(map, "C");
	desc_D = get_node_with_label(map, "D");
	desc_E = get_node_with_label(map, "E");
	desc_f = get_node_with_label(map, "f");
	desc_g = get_node_with_label(map, "g");
	desc_h = get_node_with_label(map, "h");
	desc_i = get_node_with_label(map, "i");

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

int test_lca()
{
	const char *test_name = "test_lca";

	struct rooted_tree tree;
	struct rnode *ancestor, *desc_A, *desc_B;
	struct rnode *desc_C, *desc_D, *desc_E;
	struct rnode *desc_f, *desc_g, *desc_h, *desc_i;
	struct node_map *map;
	struct llist *descendants;

	tree = tree_2();	/* ((A,B)f,(C,(D,E)g)h)i; - see tree_stubs.h */
	map = create_node_map(tree.nodes_in_order);
	desc_A = get_node_with_label(map, "A");
	desc_B = get_node_with_label(map, "B");
	desc_C = get_node_with_label(map, "C");
	desc_D = get_node_with_label(map, "D");
	desc_E = get_node_with_label(map, "E");
	desc_f = get_node_with_label(map, "f");
	desc_g = get_node_with_label(map, "g");
	desc_h = get_node_with_label(map, "h");
	desc_i = get_node_with_label(map, "i");

	descendants = create_llist();
	append_element(descendants, desc_A);
	append_element(descendants, desc_B);

	ancestor = lca(&tree, descendants);
	if (desc_f != ancestor) {
		printf ("%s: expected node 'f' as LCA of 'A' and 'B' (got '%s').\n",
				test_name, ancestor->label);
		return 1;
	}

	descendants = create_llist();
	append_element(descendants, desc_C);
	append_element(descendants, desc_D);
	append_element(descendants, desc_E);

	ancestor = lca(&tree, descendants);
	if (desc_h != ancestor) {
		printf ("%s: expected node 'h' as LCA of 'C', 'D' and 'E' (got '%s').\n",
				test_name, ancestor->label);
		return 1;
	}
	ancestor = lca2(&tree, desc_A, desc_A);
	if (desc_A != ancestor) {
		printf ("%s: expected node 'A' as LCA of 'A' and 'A' (got '%s').\n",
				test_name, ancestor->label);
		return 1;
	}
	ancestor = lca2(&tree, desc_A, desc_C);
	if (desc_i != ancestor) {
		printf ("%s: expected node 'i' as LCA of 'A' and 'C' (got '%s').\n",
				test_name, ancestor->label);
		return 1;
	}
	ancestor = lca2(&tree, desc_C, desc_A);
	if (desc_i != ancestor) {
		printf ("%s: expected node 'i' as LCA of 'C' and 'A' (got '%s').\n",
				test_name, ancestor->label);
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
	failures += test_lca(); 
	if (0 == failures) {
		printf("All tests ok.\n");
	} else {
		printf("%d test(s) FAILED.\n", failures);
		return 1;
	}

	return 0;
}
