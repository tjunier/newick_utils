#include <stdio.h>
#include <string.h>

#include "rnode.h"
#include "redge.h"
#include "link.h"
#include "list.h"
#include "tree_stubs.h"
#include "tree.h"
#include "nodemap.h"
#include "to_newick.h"


int test_reroot()
{
	const char *test_name = "test_reroot";
	struct rooted_tree tree = tree_3();	/* ((A:1,B:1.0)f:2.0,(C:1,(D:1,E:1)g:2)h:3)i; */
	struct node_map *map = create_node_map(tree.nodes_in_order);	
	struct rnode *node_g = get_node_with_label(map, "g");
	const char *exp = "((D:1,E:1)g:1,(C:1,(A:1,B:1.0)f:5)h:1);";

	reroot_tree(&tree, node_g);

	const char *obt = to_newick(tree.root);
	
	if (strcmp (exp, obt) != 0) {
		printf ("%s: expected '%s', got '%s'.\n", test_name, 
				exp, obt);
		return 1;
	}

	printf ("%s: ok.\n", test_name);
	return 0;
}

int test_reroot_2()
{
	const char *test_name = "test_reroot_2";

	/* A tree whose root has 3 children: (A:3,B:3,(C:2,(D:1,E:1)f)g)h; */

	struct rooted_tree tree = tree_5();	
	struct node_map *map = create_node_map(tree.nodes_in_order);	
	struct rnode *node_f = get_node_with_label(map, "f");
	const char *exp = "((D:1,E:1)f:0.5,(C:2,(A:3,B:3)h:1)g:0.5);";

	reroot_tree(&tree, node_f);

	const char *obt = to_newick(tree.root);
	
	if (strcmp (exp, obt) != 0) {
		printf ("%s: expected '%s', got '%s'.\n", test_name, 
				exp, obt);
		return 1;
	}

	printf ("%s: ok.\n", test_name);
	return 0;
}

int test_collapse_pure_clades()
{
	char *test_name = "test_collapse_pure_clade";
	char *exp = "((A:1,B:1.0)f:2.0,C:3)i;";
	struct rooted_tree tree = tree_4();

	collapse_pure_clades(&tree);
	char *obt = to_newick(tree.root);

	if (0 != strcmp(exp, obt)) {
		printf ("%s: expected %s, got %s.\n", test_name,
				exp, obt);
		return 1;
	}

	printf ("%s: ok.\n", test_name);
	return 0;
}

int test_leaf_count()
{
	const char *test_name = "test_leaf_count";
	struct rooted_tree tree = tree_3();	/* ((A:1,B:1.0)f:2.0,(C:1,(D:1,E:1)g:2)h:3)i; */

	if (leaf_count(&tree) != 5) {
		printf ("%s: leaf count should be 4, not %d\n", test_name,
				leaf_count(&tree));
		return 1;
	}

	printf ("%s: ok.\n", test_name);
	return 0;
}

int main()
{
	int failures = 0;
	printf("Starting tree test...\n");
	failures += test_reroot();
	failures += test_reroot_2();
	failures += test_collapse_pure_clades();
	failures += test_leaf_count();
	if (0 == failures) {
		printf("All tests ok.\n");
	} else {
		printf("%d test(s) FAILED.\n", failures);
		return 1;
	}

	return 0;
}
