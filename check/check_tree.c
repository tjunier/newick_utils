#include <stdlib.h>
#include <string.h>
#include <check.h>

#include "../src/tree.h"
#include "../src/redge.h"
#include "../src/link.h"
#include "../src/list.h"
#include "../src/nodemap.h"
#include "../src/to_newick.h"
#include "tree_stubs.h"


START_TEST(test_reroot)
{
	const char *test_name = "test_reroot";
	struct rooted_tree tree = tree_3();	/* ((A:1,B:1.0)f:2.0,(C:1,(D:1,E:1)g:2)h:3)i; */
	struct node_map *map = create_node_map(tree.nodes_in_order);	
	struct rnode *node_g = get_node_with_label(map, "g");
	const char *exp = "((D:1,E:1)g:1,(C:1,(A:1,B:1.0)f:5)h:1);";

	reroot_tree(&tree, node_g);

	const char *obt = to_newick(tree.root);
	
	fail_if (strcmp (exp, obt) != 0, "rerooting is wrong");
}
END_TEST

START_TEST(test_collapse_pure_clades)
{
	char *test_name = "test_collapse_pure_clade";
	char *exp = "((A:1,B:1.0)f:2.0,C:3)i;";
	struct rooted_tree tree = tree_4();

	collapse_pure_clades(&tree);
	char *obt = to_newick(tree.root);

	fail_if (0 != strcmp(exp, obt), "collapse wrong");
}
END_TEST

Suite * tree_suite (void)
{
	Suite *s = suite_create ("tree");

	TCase *tc_core = tcase_create ("main");
	tcase_add_test (tc_core, test_reroot);
	tcase_add_test (tc_core, test_collapse_pure_clades);
	suite_add_tcase (s, tc_core);

	return s;
}

int main (void)
{
   	int number_failed;
       Suite *s = tree_suite ();
       SRunner *sr = srunner_create (s);
       srunner_run_all (sr, CK_ENV);
       number_failed = srunner_ntests_failed (sr);
       srunner_free (sr);
       return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}


