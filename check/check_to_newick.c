#include <stdlib.h>
#include <string.h>
#include <check.h>

#include "../src/rnode.h"
#include "../src/redge.h"
#include "../src/to_newick.h"
#include "../src/link.h"

START_TEST(test_trivial)
{
	const char *test_name = "test_trivial";
	char *result;
	struct rnode *node_a;
	char * exp = "a;";

	node_a = create_rnode("a");
	is_leaf(node_a);
	result = to_newick(node_a);
	fail_if (strcmp(exp, result) != 0, "expected 'a;'");
}
END_TEST

START_TEST(test_simple_1)
{
	const char *test_name = "test_simple_1";
	char *result;
	struct rnode *node_a;
	struct rnode *node_b;
	char *exp = "(a:12);";

	node_a = create_rnode("a");
	node_b = create_rnode("");
	link_p2c(node_b, node_a, "12");

	result = to_newick(node_b);
	fail_if (strcmp(exp, result) != 0, "expected '(a:12);'");
}
END_TEST

START_TEST(test_simple_2)
{
	const char *test_name = "test_simple_2";
	char *result;
	struct rnode *node_a;
	struct rnode *node_b;
	struct rnode *node_c;
	char *exp = "(a:12,b:5);";

	node_a = create_rnode("a");
	node_b = create_rnode("b");
	node_c = create_rnode("");
	link_p2c(node_c, node_a, "12");
	link_p2c(node_c, node_b, "5");

	result = to_newick(node_c);
	fail_if (strcmp(exp, result) != 0, "expected '(a:12,b:5);'");
}
END_TEST

/* (a,(b,c)d)e; */
START_TEST(test_nested_1)
{
	const char *test_name = "test_nested_1";
	char *result;
	struct rnode *node_a;
	struct rnode *node_b;
	struct rnode *node_c;
	struct rnode *node_d;
	struct rnode *node_e;
	char *exp = "(a,(b,c));";

	node_a = create_rnode("a");
	node_b = create_rnode("b");
	node_c = create_rnode("c");
	node_d = create_rnode("");
	node_e = create_rnode("");

	link_p2c(node_d, node_b, NULL);
	link_p2c(node_d, node_c, NULL);
	link_p2c(node_e, node_a, NULL);
	link_p2c(node_e, node_d, NULL);

	result = to_newick(node_e);
	fail_if (strcmp(exp, result) != 0, "wrong tree");
}
END_TEST

/* (a,(b,c)d)e; */
START_TEST(test_nested_2)
{
	const char *test_name = "test_nested_2";
	char *result;
	struct rnode *node_a;
	struct rnode *node_b;
	struct rnode *node_c;
	struct rnode *node_d;
	struct rnode *node_e;
	char *exp = "(a:12,(b:5,c:7)d:3)e;";

	node_a = create_rnode("a");
	node_b = create_rnode("b");
	node_c = create_rnode("c");
	node_d = create_rnode("d");
	node_e = create_rnode("e");

	link_p2c(node_d, node_b, "5");
	link_p2c(node_d, node_c, "7");
	link_p2c(node_e, node_a, "12");
	link_p2c(node_e, node_d, "3");

	result = to_newick(node_e);
	fail_if (strcmp(exp, result) != 0, "wrong tree");
}
END_TEST

/* bug 1 - this bug appeared when testing other functions */

START_TEST(test_bug1)
{
	const char *test_name = "test_bug1";

	struct rnode *parent;
	struct rnode *child_1;
	struct rnode *child_2;
	struct rnode *child_3;
	struct rnode *child_4;
	struct redge *child_1_edge;
	struct redge *child_2_edge;
	struct redge *child_3_edge;
	struct redge *child_4_edge;

	parent = create_rnode("parent");
	child_1 = create_rnode("child_1");
	child_2 = create_rnode("child_2");
	child_3 = create_rnode("child_3");
	child_4 = create_rnode("child_4");
	child_1_edge = create_redge("");
	child_2_edge = create_redge("");
	child_3_edge = create_redge("");
	child_4_edge = create_redge("");

	set_parent_edge(child_1, child_1_edge);
	set_parent_edge(child_2, child_2_edge);
	set_parent_edge(child_3, child_3_edge);
	set_parent_edge(child_4, child_4_edge);

	add_child_edge(parent, child_1_edge);
	add_child_edge(parent, child_2_edge);
	add_child_edge(parent, child_3_edge);
	add_child_edge(parent, child_4_edge);
	
	struct rnode *new_child = create_rnode("new");
	struct redge *new_edge = create_redge("");
	set_parent_edge(new_child, new_edge);

	replace_child_edge(parent, child_3_edge, new_edge);

	char *exp = "(child_1,child_2,new,child_4)parent;";
	fail_if (0 != strcmp(exp, to_newick(parent)), "wrong tree");
}
END_TEST

Suite * to_newick_suite (void)
{
	Suite *s = suite_create ("to_newick");

	TCase *tc_core = tcase_create ("main");
	tcase_add_test(tc_core, test_trivial);
	tcase_add_test(tc_core, test_simple_1);
	tcase_add_test(tc_core, test_simple_2);
	tcase_add_test(tc_core, test_nested_1);
	tcase_add_test(tc_core, test_nested_2);
	tcase_add_test(tc_core, test_bug1);
	suite_add_tcase (s, tc_core);

	return s;
}

int main (void)
{
   	int number_failed;
	Suite *s = to_newick_suite ();
	SRunner *sr = srunner_create (s);
	srunner_run_all (sr, CK_ENV);
	number_failed = srunner_ntests_failed (sr);
	srunner_free (sr);
	return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}


