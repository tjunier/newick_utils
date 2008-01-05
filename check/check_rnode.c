#include <stdlib.h>
#include <check.h>

#include "../src/rnode.h"

START_TEST (test_create_rnode)
{
	struct rnode *nodep;
	char *label = "test";
	nodep = create_rnode(label);
	fail_if(NULL == nodep, "node created NULL");
	fail_if(NULL == nodep->children, "children list is NULL (should be empty, but not NULL");
	fail_unless (NULL == nodep->parent_edge, "parent node should be NULL");
	fail_unless(0 == strcmp(nodep->label, label), "label wrongly set");
}
END_TEST

START_TEST (test_create_rnode_emptylabel)
{
	struct rnode *nodep;
	char *label = "";
	nodep = create_rnode(label);
	fail_if(NULL == nodep, "node created NULL");
	fail_if(NULL == nodep->children, "children list is NULL (should be empty, but not NULL");
	fail_unless (NULL == nodep->parent_edge, "parent node should be NULL");
	fail_unless(0 == strcmp(nodep->label, label), "label wrongly set");
}
END_TEST

START_TEST (test_create_many)
{
	int i;
	int num = 100000;
	for (i = 0; i < num; i++) {
		struct rnode *nodep;
		nodep = create_rnode("test");
	}
}
END_TEST

Suite * rnode_suite (void)
{
	Suite *s = suite_create ("rnode");

	TCase *tc_core = tcase_create ("main");
	tcase_add_test (tc_core, test_create_rnode);
	tcase_add_test (tc_core, test_create_rnode_emptylabel);
	tcase_add_test (tc_core, test_create_many);
	suite_add_tcase (s, tc_core);

	return s;
}

int main (void)
{
   	int number_failed;
       Suite *s = rnode_suite ();
       SRunner *sr = srunner_create (s);
       srunner_run_all (sr, CK_VERBOSE);
       number_failed = srunner_ntests_failed (sr);
       srunner_free (sr);
       return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}


