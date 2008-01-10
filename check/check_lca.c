#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <check.h>

#include "../src/lca.h"
#include "tree_stubs.h"
#include "../src/to_newick.h"
#include "../src/rnode.h"
#include "../src/nodemap.h"
#include "../src/list.h"
#include "../src/tree.h"

START_TEST(test_lca)
{
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
	fail_if (desc_f != ancestor, "expected node 'f' as LCA of 'A' and 'B'");

	descendants = create_llist();
	append_element(descendants, desc_C);
	append_element(descendants, desc_D);
	append_element(descendants, desc_E);

	ancestor = lca(&tree, descendants);
	fail_if (desc_h != ancestor, "expected node 'h' as LCA of 'C', 'D', and 'E'");
}
END_TEST

START_TEST(test_lca2)
{
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
	fail_if (desc_f != lca, "expected node 'f' as LCA of 'A' and 'B'");
	lca = lca2(&tree, desc_D, desc_E);
	fail_if (desc_g != lca, "expected node 'g' as LCA of 'D' and 'E'");
	lca = lca2(&tree, desc_A, desc_A);
	fail_if (desc_A != lca, "expected node 'A' as LCA of 'A' and 'A'");
	lca = lca2(&tree, desc_A, desc_C);
	fail_if (desc_i != lca, "expected node 'i' as LCA of 'A' and 'C'");
	lca = lca2(&tree, desc_C, desc_A);
	fail_if (desc_i != lca, "expected node 'i' as LCA of 'C' and 'A'");
	lca = lca2(&tree, desc_h, desc_f);
	fail_if (desc_i != lca, "expected node 'i' as LCA of 'h' and 'f'");
	lca = lca2(&tree, desc_h, desc_E);
	fail_if (desc_h != lca, "expected node 'i' as LCA of 'h' and 'E'");
}
END_TEST

Suite * lca_suite (void)
{
	Suite *s = suite_create ("lca");

	TCase *tc_core = tcase_create ("main");
	tcase_add_test(tc_core, test_lca2);
	tcase_add_test(tc_core, test_lca);
	suite_add_tcase (s, tc_core);

	return s;
}

int main (void)
{
   	int number_failed;
	Suite *s = lca_suite ();
	SRunner *sr = srunner_create (s);
	srunner_run_all (sr, CK_ENV);
	number_failed = srunner_ntests_failed (sr);
	srunner_free (sr);
	return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}


