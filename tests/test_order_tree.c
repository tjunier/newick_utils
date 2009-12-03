#include <stdio.h>
#include <string.h>

#include "tree.h"
#include "order_tree.h"
#include "tree_stubs.h"
#include "to_newick.h"

int test_order_lbl()
{
	const char *test_name = "test_order_lbl";
	struct rooted_tree tree = tree_13();
	order_tree_lbl(&tree);
	char *obt_newick = to_newick(tree.root);
	char *exp_newick = "((Ant,Bee),((Cat,Dog),Eel));";

	if (0 != strcmp(obt_newick, exp_newick)) {
		printf ("%s: expected '%s', got '%s'.\n",
			test_name, exp_newick,
			obt_newick);
		return 1;
	}

	printf("%s ok.\n", test_name);
	return 0;
}

int test_order_num_desc()
{
	const char *test_name = "test_order_num_desc";
	/* tree is top-heavy */
	struct rooted_tree test_tree = tree_15();
	/* expected tree is top-light */
	struct rooted_tree exp_tree = tree_14();
	order_tree_num_desc(&test_tree);
	char *obt_newick = to_newick(test_tree.root);
	char *exp_newick = to_newick(exp_tree.root);

	if (0 != strcmp(obt_newick, exp_newick)) {
		printf ("%s: expected '%s', got '%s'.\n",
			test_name, exp_newick,
			obt_newick);
		return 1;
	}

	printf("%s ok.\n", test_name);
	return 0;
}

int test_order_deladderize()
{
	const char *test_name = "test_order_num_desc";
	struct rooted_tree test_tree = tree_15();
	order_tree_num_desc(&test_tree);
	char *obt_newick = to_newick(test_tree.root);
	char *exp_newick = "(Petromyzon,((Xenopus,((Equus,Homo)Mammalia,Columba)Amniota)Tetrapoda,Carcharodon)Gnathostomata)Vertebrata;";


	if (0 != strcmp(obt_newick, exp_newick)) {
		printf ("%s: expected '%s', got '%s'.\n",
			test_name, exp_newick,
			obt_newick);
		return 1;
	}

	printf("%s ok.\n", test_name);
	return 0;
}

int main()
{
	int failures = 0;
	printf("Starting reordering test...\n");
	failures += test_order_lbl();
	failures += test_order_num_desc();
	if (0 == failures) {
		printf("All tests ok.\n");
	} else {
		printf("%d test(s) FAILED.\n", failures);
		return 1;
	}

	return 0;
}
