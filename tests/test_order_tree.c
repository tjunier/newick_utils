#include <stdio.h>
#include <string.h>

#include "tree.h"
#include "order_tree.h"
#include "tree_stubs.h"
#include "to_newick.h"
#include "nodemap.h"
#include "hash.h"
#include "rnode.h"

int test_order()
{
	const char *test_name = __func__;
	/* ((Bee,Ant),(Eel,(Dog,Cat))); */
	struct rooted_tree tree = tree_13();
	struct hash *map = create_label2node_map(tree.nodes_in_order);
	struct rnode *node_Ant = hash_get(map, "Ant");
	struct rnode *node_Bee = hash_get(map, "Bee");
	struct rnode *node_Cat = hash_get(map, "Cat");
	struct rnode *node_Dog = hash_get(map, "Dog");
	struct rnode *node_Eel = hash_get(map, "Eel");
	/* These nodes have no label, so we address them indirectly */
	struct rnode *node_insects = tree.root->first_child;
	struct rnode *node_vertebrates = tree.root->last_child;
	struct rnode *node_carnivores = node_vertebrates->last_child;

	order_tree(&tree, lbl_comparator, set_sort_field_label);
	
	/* insect node should still be 1st */
	if (tree.root->first_child != node_insects) {
		printf ("%s: insects should still be root's 1st child.\n",
				test_name);
		return 1;
	}
	/* vertebrate node should still be last */
	if (tree.root->last_child != node_vertebrates) {
		printf ("%s: vertebrates should still be root's last child.\n",
				test_name);
		return 1;
	}
	if (node_insects->first_child != node_Ant) {
		printf ("%s: first insect should be Ant, but is %s\n",
				test_name, node_insects->first_child->label);
		return 1;
	}
	if (node_insects->last_child != node_Bee) {
		printf ("%s: last insect should be Bee, but is %s\n",
				test_name, node_insects->last_child->label);
		return 1;
	}
	if (node_Ant->next_sibling != node_Bee) {
		printf ("%s: Ant's next sib should be Bee, but is %s\n",
				test_name, node_Ant->next_sibling->label);
		return 1;
	}
	if (NULL != node_Bee->next_sibling) {
		printf ("%s: Bee should have no next sibling\n", test_name);
		return 1;
	}

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
	const char *test_name = __func__;
	/* tree is top-heavy */
	struct rooted_tree test_tree = tree_15();
	/* expected tree is top-light */
	struct rooted_tree exp_tree = tree_14();
	order_tree(&test_tree, num_desc_comparator, set_sort_field_num_desc);
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
	const char *test_name = __func__;
	struct rooted_tree test_tree = tree_15();
	order_tree(&test_tree, num_desc_deladderize, set_sort_field_num_desc);
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
	failures += test_order();
	failures += test_order_num_desc();
	failures += test_order_deladderize();
	if (0 == failures) {
		printf("All tests ok.\n");
	} else {
		printf("%d test(s) FAILED.\n", failures);
		return 1;
	}

	return 0;
}
