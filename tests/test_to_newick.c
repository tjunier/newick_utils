#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "rnode.h"
#include "redge.h"
#include "to_newick.h"
#include "link.h"

int test_trivial()
{
	const char *test_name = "test_trivial";
	char *result;
	struct rnode *node_a;
	char * exp = "a;";

	node_a = create_rnode("a");
	is_leaf(node_a);
	result = to_newick(node_a);
	if (strcmp(exp, result) != 0) {
		printf("%s: expected '%s', got '%s'.\n",
				test_name, exp, result);
		return 1;
	}
	printf("%s: ok.\n", test_name);
	return 0;
	
}

int test_simple_1()
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
	if (strcmp(exp, result) != 0) {
		printf("%s: expected '%s', got '%s'.\n",
				test_name, exp, result);
		return 1;
	}
	printf("%s: ok.\n", test_name);
	return 0;
}

int test_simple_2()
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
	if (strcmp(exp, result) != 0) {
		printf("%s: expected '%s', got '%s'.\n",
				test_name, exp, result);
		return 1;
	}
	printf("%s: ok.\n", test_name);
	return 0;
}

/* (a,(b,c)d)e; */
int test_nested_1()
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
	if (strcmp(exp, result) != 0) {
		printf("%s: expected '%s', got '%s'.\n",
				test_name, exp, result);
		return 1;
	}
	printf("%s: ok.\n", test_name);
	return 0;
}

/* (a,(b,c)d)e; */
int test_nested_2()
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
	if (strcmp(exp, result) != 0) {
		printf("%s: expected '%s', got '%s'.\n",
				test_name, exp, result);
		return 1;
	}
	printf("%s: ok.\n", test_name);
	return 0;
}

/* bug 1 - this bug appeared when testing other functions */

int test_bug1()
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
	if (0 != strcmp(exp, to_newick(parent))) {
		printf ("%s: expected '%s', got '%s'\n",
			test_name, exp, to_newick(parent));
		return 1;
	}

	printf("%s ok.\n", test_name);
	return 0;
}

int main()
{
	int failures = 0;
	printf("Starting to_newick test...\n");
	failures += test_trivial();
	failures += test_simple_1();
	failures += test_simple_2();
	failures += test_nested_1();
	failures += test_nested_2();
	failures += test_bug1();
	if (0 == failures) {
		printf("All tests ok.\n");
	} else {
		printf("%d test(s) FAILED.\n", failures);
		return 1;
	}

	return 0;
}
