#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>

#include "rnode.h"
#include "to_newick.h"
#include "link.h"
#include "concat.h"
#include "parser.h"
#include "tree.h"
#include "list.h"
#include "tree_stubs.h"

void newick_scanner_set_string_input(char *);

int test_trivial()
{
	const char *test_name = "test_trivial";
	char *result;
	struct rnode *node_a;
	char * exp = "a;";

	node_a = create_rnode("a", "");
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

	node_a = create_rnode("a", "12");
	node_b = create_rnode("", "");
	add_child(node_b, node_a);

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

	node_a = create_rnode("a", "12");
	node_b = create_rnode("b", "5");
	node_c = create_rnode("", "");
	add_child(node_c, node_a);
	add_child(node_c, node_b);

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

	node_a = create_rnode("a", NULL);
	node_b = create_rnode("b", NULL);
	node_c = create_rnode("c", NULL);
	node_d = create_rnode("", NULL);
	node_e = create_rnode("", NULL);

	add_child(node_d, node_b);
	add_child(node_d, node_c);
	add_child(node_e, node_a);
	add_child(node_e, node_d);

	result = to_newick(node_e);
	if (strcmp(exp, result) != 0) {
		printf("%s: expected '%s', got '%s'.\n",
				test_name, exp, result);
		return 1;
	}
	printf("%s: ok.\n", test_name);
	return 0;
}

/* (a:12,(b:5,c:7)d:3)e; */
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

	node_a = create_rnode("a", "12");
	node_b = create_rnode("b", "5");
	node_c = create_rnode("c", "7");
	node_d = create_rnode("d", "3");
	node_e = create_rnode("e", "");

	add_child(node_d, node_b);
	add_child(node_d, node_c);
	add_child(node_e, node_a);
	add_child(node_e, node_d);

	result = to_newick(node_e);
	if (strcmp(exp, result) != 0) {
		printf("%s: expected '%s', got '%s'.\n",
				test_name, exp, result);
		return 1;
	}
	printf("%s: ok.\n", test_name);
	return 0;
}

/* bug 1 - these bugs appeared when testing other functions */

int test_bug1()
{
	const char *test_name = "test_bug1";

	struct rnode *parent;
	struct rnode *child_1;
	struct rnode *child_2;
	struct rnode *child_3;
	struct rnode *child_4;

	parent = create_rnode("parent", "");
	child_1 = create_rnode("child_1", "");
	child_2 = create_rnode("child_2", "");
	child_3 = create_rnode("child_3", "");
	child_4 = create_rnode("child_4", "");

	add_child(parent, child_1);
	add_child(parent, child_2);
	add_child(parent, child_3);
	add_child(parent, child_4);
	
	struct rnode *new_child = create_rnode("new", "");

	replace_child(child_3, new_child);

	char *exp = "(child_1,child_2,new,child_4)parent;";
	if (0 != strcmp(exp, to_newick(parent))) {
		printf ("%s: expected '%s', got '%s'\n",
			test_name, exp, to_newick(parent));
		return 1;
	}

	printf("%s ok.\n", test_name);
	return 0;
}

int test_bug2()
{
	char *test_name = "test_bug2";
	char *newick = "(Bovine:0.69395,(Gibbon:0.36079,(Orang:0.33636,(Gorilla:0.17147,(Chimp:0.19268,Human:0.11927):0.08386):0.06124):0.15057):0.54939,Mouse:1.21460):0.10;";

	newick_scanner_set_string_input(newick);

	struct rooted_tree *tree = parse_tree();
	struct rnode *root = tree->root;
	if (strcmp("", root->label) != 0) {
		printf("%s: expected empty root label, got '%s'\n",
				test_name, root->label);
		return 1;
	}
	if (strcmp("0.10", root->edge_length_as_string) != 0) {
		printf("%s: expected root edge length 0.10 , got '%s'\n",
				test_name, root->edge_length_as_string);
		return 1;
	}
	char *obt = to_newick(tree->root);
	if (strcmp(obt, newick) != 0) {
		printf ("%s: expected '%s', got '%s'\n", test_name,
				newick, obt);
		return 1;
	}

	printf("%s ok.\n", test_name);
	return 0;
}

int test_bug3()
{
	const char *test_name = __func__;
	/* ((Bee,Ant)f,(Eel,(Dog,Cat)g)h)i; */
	struct rooted_tree tree = tree_13();
	char *obt_newick = to_newick(tree.root);
	char *exp_newick = "((Bee,Ant),(Eel,(Dog,Cat)));";

	if (0 != strcmp(obt_newick, exp_newick)) {
		printf ("%s: expected '%s', got '%s'.\n",
			test_name, exp_newick,
			obt_newick);
		return 1;
	}

	printf("%s ok.\n", test_name);
	return 0;
}

/* a:12; */
int test_to_newick_i_leaf()
{
	const char *test_name = __func__;
	struct llist *result;
	struct rnode *node_a;
	struct list_elem *e;

	/* (a:12); */
	node_a = create_rnode("a", "12");

	result = to_newick_i(node_a);
	e = result->head;
	if (NULL == e) {
		printf ("%s: list head is NULL\n", test_name);
		return 1;
	}
	if (strcmp("a", (char *)(e->data)) != 0) {
		printf ("%s: expected 'a', got '%s'\n", test_name, (char*)(e->data));
		return 1;
	}
	e = e->next;
	if (NULL == e) {
		printf ("%s: list element is NULL\n", test_name);
		return 1;
	}
	if (strcmp(":", (char *)(e->data)) != 0) {
		printf ("%s: expected ':', got '%s'\n", test_name, (char*)(e->data));
		return 1;
	}
	e = e->next;
	if (NULL == e) {
		printf ("%s: list element is NULL\n", test_name);
		return 1;
	}
	if (strcmp("12", (char *)(e->data)) != 0) {
		printf ("%s: expected '12', got '%s'\n", test_name, (char*)(e->data));
		return 1;
	}
	e = e->next;
	if (NULL == e) {
		printf ("%s: list element is NULL\n", test_name);
		return 1;
	}
	if (strcmp(";", (char *)(e->data)) != 0) {
		printf ("%s: expected ';', got '%s'\n", test_name, (char*)(e->data));
		return 1;
	}
	e = e->next;
	if (NULL != e) {
		printf ("%s: list element should be NULL\n", test_name);
		return 1;
	}

	printf("%s: ok.\n", test_name);
	return 0;
}

/* (a:12); */
int test_to_newick_i_simple()
{
	const char *test_name = __func__;
	struct llist *result;
	struct rnode *node_a;
	struct rnode *node_b;
	struct list_elem *e;

	/* (a:12); */
	node_a = create_rnode("a", "12");
	node_b = create_rnode("", "");
	add_child(node_b, node_a);

	result = to_newick_i(node_b);
	e = result->head;
	if (NULL == e) {
		printf ("%s: list head is NULL\n", test_name);
		return 1;
	}
	if (strcmp("(", (char *)(e->data)) != 0) {
		printf ("%s: expected '(', got '%s'\n", test_name, (char*)(e->data));
		return 1;
	}
	e = e->next;
	if (NULL == e) {
		printf ("%s: list element is NULL\n", test_name);
		return 1;
	}
	if (strcmp("a", (char *)(e->data)) != 0) {
		printf ("%s: expected 'a', got '%s'\n", test_name, (char*)(e->data));
		return 1;
	}
	e = e->next;
	if (NULL == e) {
		printf ("%s: list element is NULL\n", test_name);
		return 1;
	}
	if (strcmp(":", (char *)(e->data)) != 0) {
		printf ("%s: expected ':', got '%s'\n", test_name, (char*)(e->data));
		return 1;
	}
	e = e->next;
	if (NULL == e) {
		printf ("%s: list element is NULL\n", test_name);
		return 1;
	}
	if (strcmp("12", (char *)(e->data)) != 0) {
		printf ("%s: expected '12', got '%s'\n", test_name, (char*)(e->data));
		return 1;
	}
	e = e->next;
	if (NULL == e) {
		printf ("%s: list element is NULL\n", test_name);
		return 1;
	}
	if (strcmp(")", (char *)(e->data)) != 0) {
		printf ("%s: expected ')', got '%s'\n", test_name, (char*)(e->data));
		return 1;
	}
	e = e->next;
	if (NULL == e) {
		printf ("%s: list element is NULL\n", test_name);
		return 1;
	}
	if (strcmp(";", (char *)(e->data)) != 0) {
		printf ("%s: expected ';', got '%s'\n", test_name, (char*)(e->data));
		return 1;
	}
	e = e->next;
	if (NULL != e) {
		printf ("%s: list element should be NULL\n", test_name);
		return 1;
	}

	printf("%s: ok.\n", test_name);
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
	failures += test_bug2();
	failures += test_bug3();
	failures += test_to_newick_i_leaf();
	failures += test_to_newick_i_simple();
	if (0 == failures) {
		printf("All tests ok.\n");
	} else {
		printf("%d test(s) FAILED.\n", failures);
		return 1;
	}

	return 0;
}
