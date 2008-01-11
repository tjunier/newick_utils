#include <stdlib.h>
#include <string.h>
#include <check.h>

#include "../src/nodemap.h"
#include "../src/rnode.h"
#include "../src/list.h"

START_TEST (test_create_and_retrieve)
{

	struct rnode *n1, *n2, *n3;
	struct llist *node_list;
	struct node_map *map;

	n1 = create_rnode("n1");
	n2 = create_rnode("n2");
	n3 = create_rnode("n3");
	node_list = create_llist();
	append_element(node_list, n1);
	append_element(node_list, n2);
	append_element(node_list, n3);
	map = create_node_map(node_list);

	fail_if (NULL == map, "*map must not be NULL");
	fail_if (NULL != get_node_with_label(map, "not there"), "inexistent label should return NULL");
	fail_if (n1 != get_node_with_label(map, "n1"), "should get node with label 'n1'");
	fail_if (n2 != get_node_with_label(map, "n2"), "should get node with label 'n2'");
	fail_if (n3 != get_node_with_label(map, "n3"), "should get node with label 'n3'");
}
END_TEST

Suite * nodemap_suite (void)
{
	Suite *s = suite_create ("nodemap");

	TCase *tc_core = tcase_create ("main");
	tcase_add_test(tc_core, test_create_and_retrieve);
	suite_add_tcase (s, tc_core);

	return s;
}

int main (void)
{
   	int number_failed;
	Suite *s = nodemap_suite ();
	SRunner *sr = srunner_create (s);
	srunner_run_all (sr, CK_ENV);
	number_failed = srunner_ntests_failed (sr);
	srunner_free (sr);
	return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}


