#include <stdio.h>
#include <string.h>

#include "rnode.h"
#include "tree.h"
#include "hash.h"
#include "nodemap.h"
#include "tree_stubs.h"
#include "link.h"
#include "list.h"

int test_create_rnode()
{
	const char *test_name = "test_create_rnode";
	struct rnode *node;
	char *label = "test";
	char *length = "2.0456";
	node = create_rnode(label,length);
	if (0 != strcmp(node->label, label)) {
		printf("%s: expected label '%s' (got '%s')\n",
				test_name, label, node->label);
		return 1;
	}
	if (0 != strcmp(node->edge_length_as_string, length)) {
		printf("%s: expected length '%s', got '%s'\n",
			test_name, length, node->edge_length_as_string);
		return 1;
	}
	if (NULL != node->parent) {
		printf ("%s: parent should be NULL.\n", test_name);
		return 1;
	}
	if (NULL != node->next_sibling) {
		printf ("%s: next_sibling should be NULL.\n", test_name);
		return 1;
	}
	if (NULL != node->first_child) {
		printf ("%s: first_child should be NULL.\n", test_name);
		return 1;
	}
	if (NULL != node->last_child) {
		printf ("%s: last_child should be NULL.\n", test_name);
		return 1;
	}
	if (0 != node->child_count) {
		printf("%s: 'child_count' should be 0 (got %d)\n", test_name,
				node->child_count);
		return 1;
	}
	if (0 != node->seen) {
		printf("%s: 'seen' should be 0 (got %d)\n", test_name,
				node->seen);
		return 1;
	}
	if (NULL != node->current_child) {
		printf("%s: 'current_child_elem' should be NULL (got %p)\n", test_name,
				node->current_child);
		return 1;
	}
	printf("%s ok.\n", test_name);
	return 0;
}

int test_static_rnode_vars()
{
	const char *test_name = __func__;

	/* Start with a blank slate: previous tests have allocated nodes. */
	destroy_all_rnodes(NULL);
	/* Node count should now be zero. */
	if (0 != _get_rnode_count()) {
		printf("%s: expected node count of 0, got %d.\n",
				test_name, _get_rnode_count());
		return 1;
	}
	/* create 10 nodes, then check the vars */
	int i;
	for (i = 0; i < 10; i++) {
		 create_rnode("","");
		 // printf("[%s]: %d nodes created.\n", __func__, _get_rnode_count());
	}

	if (10 != _get_rnode_count()) {
		printf("%s: expected node count of 10, got %d.\n",
				test_name, _get_rnode_count());
		return 1;
	}
	destroy_all_rnodes(NULL);
	if (0 != _get_rnode_count()) {
		printf("%s: expected node count of 0, got %d.\n",
				test_name, _get_rnode_count());
		return 1;
	}

	printf("%s ok.\n", test_name);
	return 0;
}

int test_static_rnode_vars_2()
{
	const char *test_name = __func__;
	struct rooted_tree tree;

	/* Start with a blank slate: previous tests have allocated nodes. */
	destroy_all_rnodes(NULL);
	/* Node count should now be zero. */
	if (0 != _get_rnode_count()) {
		printf("%s: expected node count of 0, got %d.\n",
				test_name, _get_rnode_count());
		return 1;
	}
	/* create tree */
	tree = tree_8();
	if (9 != _get_rnode_count()) {
		printf("%s: expected node count of 9, got %d.\n",
				test_name, _get_rnode_count());
		return 1;
	}
	destroy_all_rnodes(NULL);
	if (0 != _get_rnode_count()) {
		printf("%s: expected node count of 0, got %d.\n",
				test_name, _get_rnode_count());
		return 1;
	}

	printf("%s ok.\n", test_name);
	return 0;
}

int test_create_rnode_nulllabel()
{
	const char *test_name = "test_create_rnode_nulllabel";
	struct rnode *node;
	node = create_rnode(NULL,"");
	if (0 != strcmp(node->label, "")) {
		printf("%s: expected empty label (""), got '%s'\n",
				test_name, node->label);
		return 1;
	}
	if (0 != strcmp(node->edge_length_as_string, "")) {
		printf("%s: expected empty length, got '%s'\n",
			test_name, node->edge_length_as_string);
		return 1;
	}
	if (NULL != node->parent) {
		printf ("%s: parent node should be NULL.\n", test_name);
		return 1;
	}
	if (NULL != node->next_sibling) {
		printf ("%s: next_sibling should be NULL.\n", test_name);
		return 1;
	}
	if (NULL != node->first_child) {
		printf ("%s: first_child should be NULL.\n", test_name);
		return 1;
	}
	if (NULL != node->last_child) {
		printf ("%s: last_child should be NULL.\n", test_name);
		return 1;
	}
	if (0 != node->seen) {
		printf("%s: 'seen' should be 0 (got %d)\n", test_name,
				node->seen);
		return 1;
	}
	if (NULL != node->current_child) {
		printf("%s: 'current_child_elem' should be NULL (got %p)\n", test_name, node->current_child);
		return 1;
	}
	printf("%s ok.\n", test_name);
	return 0;
}

int test_create_rnode_emptylabel()
{
	const char *test_name = "test_create_rnode_emptylabel";
	struct rnode *node;
	node = create_rnode("","");
	if (0 != strcmp(node->label, "")) {
		printf("%s: expected empty label (""), got '%s'\n",
				test_name, node->label);
		return 1;
	}
	if (0 != strcmp(node->edge_length_as_string, "")) {
		printf("%s: expected empty length, got '%s'\n",
			test_name, node->edge_length_as_string);
		return 1;
	}
	if (NULL != node->parent) {
		printf ("%s: parent node should be NULL.\n", test_name);
		return 1;
	}
	if (NULL != node->next_sibling) {
		printf ("%s: next_sibling should be NULL.\n", test_name);
		return 1;
	}
	if (NULL != node->first_child) {
		printf ("%s: first_child should be NULL.\n", test_name);
		return 1;
	}
	if (NULL != node->last_child) {
		printf ("%s: last_child should be NULL.\n", test_name);
		return 1;
	}
	if (0 != node->seen) {
		printf("%s: 'seen' should be 0 (got %d)\n", test_name,
				node->seen);
		return 1;
	}
	if (NULL != node->current_child) {
		printf("%s: 'current_child_elem' should be NULL (got %p)\n", test_name,
				node->current_child);
		return 1;
	}
	printf("%s ok.\n", test_name);
	return 0;
}

int test_create_rnode_nulllength()
{
	const char *test_name = "test_create_rnode_nulllength";
	struct rnode *node;
	char *label = "test";
	node = create_rnode(label,NULL);
	if (0 != strcmp(node->label, label)) {
		printf("%s: expected label '%s' (got '%s')\n",
				test_name, label, node->label);
		return 1;
	}
	if (0 != strcmp(node->edge_length_as_string, "")) {
		printf("%s: expected empty length, got '%s'\n",
			test_name, node->edge_length_as_string);
		return 1;
	}
	if (NULL != node->parent) {
		printf ("%s: parent should be NULL.\n", test_name);
		return 1;
	}
	if (NULL != node->next_sibling) {
		printf ("%s: next_sibling should be NULL.\n", test_name);
		return 1;
	}
	if (NULL != node->first_child) {
		printf ("%s: first_child should be NULL.\n", test_name);
		return 1;
	}
	if (NULL != node->last_child) {
		printf ("%s: last_child should be NULL.\n", test_name);
		return 1;
	}
	if (0 != node->seen) {
		printf("%s: 'seen' should be 0 (got %d)\n", test_name,
				node->seen);
		return 1;
	}
	if (NULL != node->current_child) {
		printf("%s: 'current_child_elem' should be NULL (got %p)\n", test_name,
				node->current_child);
		return 1;
	}
	printf("%s ok.\n", test_name);
	return 0;
}

int test_create_rnode_emptylength()
{
	const char *test_name = "test_create_rnode_emptylength";
	struct rnode *node;
	char *label = "test";
	node = create_rnode(label,"");
	if (0 != strcmp(node->label, label)) {
		printf("%s: expected label '%s' (got '%s')\n",
				test_name, label, node->label);
		return 1;
	}
	if (0 != strcmp(node->edge_length_as_string, "")) {
		printf("%s: expected empty length, got '%s'\n",
			test_name, node->edge_length_as_string);
		return 1;
	}
	if (NULL != node->parent) {
		printf ("%s: parent should be NULL.\n", test_name);
		return 1;
	}
	if (NULL != node->next_sibling) {
		printf ("%s: next_sibling should be NULL.\n", test_name);
		return 1;
	}
	if (NULL != node->first_child) {
		printf ("%s: first_child should be NULL.\n", test_name);
		return 1;
	}
	if (NULL != node->last_child) {
		printf ("%s: last_child should be NULL.\n", test_name);
		return 1;
	}
	if (0 != node->seen) {
		printf("%s: 'seen' should be 0 (got %d)\n", test_name,
				node->seen);
		return 1;
	}
	if (NULL != node->current_child) {
		printf("%s: 'current_child_elem' should be NULL (got %p)\n", test_name,
				node->current_child);
		return 1;
	}
	printf("%s ok.\n", test_name);
	return 0;
}

int test_create_many()
{
	int i;
	int num = 100000;
	for (i = 0; i < num; i++) {
		struct rnode *node;
		node = create_rnode("test", "2.34");
	}
	printf ("test_create_many Ok (created %d).\n", num);
	return 0; 	/* crashes on failure */
}

int test_all_children_are_leaves()
{
	const char *test_name = __func__;
	/* ((A,B)f,(C,(D,E)g)h)i; */
	struct rooted_tree tree = tree_2();
	struct hash *map = create_label2node_map(tree.nodes_in_order);
	struct rnode *node_A = hash_get(map, "A");
	struct rnode *node_f = hash_get(map, "f");
	struct rnode *node_g = hash_get(map, "g");
	struct rnode *node_h = hash_get(map, "h");
	struct rnode *node_i = hash_get(map, "i");

	if (all_children_are_leaves(node_A)) {
		printf ("%s: A _is_ a leaf: f() should be false.\n", test_name);
		return 1;
	}
	if (! all_children_are_leaves(node_f)) {
		printf ("%s: all f's children are leaves.\n", test_name);
		return 1;
	}
	if (! all_children_are_leaves(node_g)) {
		printf ("%s: all g's children are leaves.\n", test_name);
		return 1;
	}
	if (all_children_are_leaves(node_h)) {
		printf ("%s: only one of h's chldren is a leaf\n", test_name);
		return 1;
	}
	if (all_children_are_leaves(node_i)) {
		printf ("%s: none of i's chldren is a leaf\n", test_name);
		return 1;
	}

	printf ("%s: ok.\n", test_name);
	return 0;
}

int test_all_children_have_same_label()
{
	const char *test_name = __func__;
	/* ((A:1,B:1.0)f:2.0,(C:1,(C:1,C:1)g:2)h:3)i;  - one clade made of
	 * three 'C's */
	struct rooted_tree tree = tree_4();
	struct hash *map = create_label2node_map(tree.nodes_in_order);
	struct rnode *node_A = hash_get(map, "A");
	struct rnode *node_f = hash_get(map, "f");
	struct rnode *node_g = hash_get(map, "g");
	struct rnode *node_i = hash_get(map, "i");

	struct rnode *mum = create_rnode("mum", "");
	struct rnode *kid1 = create_rnode("", "");
	struct rnode *kid2 = create_rnode("", "");
	add_child(mum, kid1);
	add_child(mum, kid2);

	char *label;

	if (all_children_have_same_label(node_A, &label)) {
		printf ("%s: A is a leaf: f() should be false.\n", test_name);
		return 1;
	}
	if (all_children_have_same_label(node_f, &label)) {
		printf ("%s: f's children have different labels.\n", test_name);
		return 1;
	}
	if (! all_children_have_same_label(node_g, &label)) {
		printf ("%s: all g's children have the same label.\n", test_name);
		return 1;
	}
	if (strcmp(label, "C") != 0) {
		printf("%s: shared label should be 'C'\n", test_name);
		return 1;
	}
	if (all_children_have_same_label(node_i, &label)) {
		printf ("%s: h's children have different labels.\n", test_name);
		return 1;
	}
	if (! all_children_have_same_label(mum, &label)) {
		printf ("%s: all mum's children have the same label.\n", test_name);
		return 1;
	}
	if (strcmp(label, "") != 0) {
		printf("%s: shared label should be ''\n", test_name);
		return 1;
	}

	printf ("%s: ok.\n", test_name);
	return 0;
}

int test_get_nodes_in_order()
{
	const char *test_name = "test_get_nodes_in_order";

	/* (A:3,B:3,(C:2,(D:1,E:1)f:1)g:1)h; */
	struct rooted_tree tree = tree_5();
	/* What is being tested here is the get_nodes_in_order() function,
	 * which computes the ordered nodes list from a root node (the usual
	 * way is to compute it while parsing Newick) */
	struct llist *nodes_in_order = get_nodes_in_order(tree.root);

	struct list_elem *elem = nodes_in_order->head;
	struct rnode *node = (struct rnode *)elem->data;
	if (strcmp("A", node->label)) {
		printf ("%s: expected node A, got %s.\n", test_name, node->label);
		return 1;
	}
	elem = elem->next;
	node = (struct rnode *) elem->data;
	if (strcmp("B", node->label)) {
		printf ("%s: expected node B, got %s.\n", test_name, node->label);
		return 1;
	}
	elem = elem->next;
	node = (struct rnode *) elem->data;
	if (strcmp("C", node->label)) {
		printf ("%s: expected node C, got %s.\n", test_name, node->label);
		return 1;
	}
	elem = elem->next;
	node = (struct rnode *) elem->data;
	if (strcmp("D", node->label)) {
		printf ("%s: expected node D, got %s.\n", test_name, node->label);
		return 1;
	}
	elem = elem->next;
	node = (struct rnode *) elem->data;
	if (strcmp("E", node->label)) {
		printf ("%s: expected node E, got %s.\n", test_name, node->label);
		return 1;
	}
	elem = elem->next;
	node = (struct rnode *) elem->data;
	if (strcmp("f", node->label)) {
		printf ("%s: expected node f, got %s.\n", test_name, node->label);
		return 1;
	}
	elem = elem->next;
	node = (struct rnode *) elem->data;
	if (strcmp("g", node->label)) {
		printf ("%s: expected node g, got %s.\n", test_name, node->label);
		return 1;
	}
	elem = elem->next;
	node = (struct rnode *) elem->data;
	if (strcmp("h", node->label)) {
		printf ("%s: expected node h, got %s.\n", test_name, node->label);
		return 1;
	}

	printf("%s ok.\n", test_name);
	return 0;
}

int test_get_nodes_in_order_linear()
{
	const char *test_name = "test_get_nodes_in_order_linear";

	/* (((((Homo_sapiens)Homo)Hominini)Homininae)Hominidae)Hominoidea; */
	struct rooted_tree tree = tree_10();
	struct llist *nodes_in_order = get_nodes_in_order(tree.root);
	struct list_elem *elem = nodes_in_order->head;

	struct rnode *node = (struct rnode *)elem->data;
	if (strcmp("Homo_sapiens", node->label)) {
		printf ("%s: expected node Homo_sapiens, got %s.\n", test_name, node->label);
		return 1;
	}
	elem = elem->next;
	node = (struct rnode *) elem->data;
	if (strcmp("Homo", node->label)) {
		printf ("%s: expected node Homo, got %s.\n", test_name, node->label);
		return 1;
	}
	elem = elem->next;
	node = (struct rnode *) elem->data;
	if (strcmp("Hominini", node->label)) {
		printf ("%s: expected node Hominini, got %s.\n", test_name, node->label);
		return 1;
	}
	elem = elem->next;
	node = (struct rnode *) elem->data;
	if (strcmp("Homininae", node->label)) {
		printf ("%s: expected node Homininae, got %s.\n", test_name, node->label);
		return 1;
	}
	elem = elem->next;
	node = (struct rnode *) elem->data;
	if (strcmp("Hominidae", node->label)) {
		printf ("%s: expected node Hominidae, got %s.\n", test_name, node->label);
		return 1;
	}
	elem = elem->next;
	node = (struct rnode *) elem->data;
	if (strcmp("Hominoidea", node->label)) {
		printf ("%s: expected node Hominoidea, got %s.\n", test_name, node->label);
		return 1;
	}
	elem = elem->next;
	if (NULL != elem) {
		printf ("%s: list is not terminated.\n", test_name);
		return 1;
	}

	printf("%s ok.\n", test_name);
	return 0;
}

int test_get_nodes_in_order_part_linear()
{
	const char *test_name = "test_get_nodes_in_order_part_linear";

	/* ((((Gorilla,(Pan,Homo)Hominini)Homininae)Hominidae)Hominoidea); */
	struct rooted_tree tree = tree_11();
	struct llist *nodes_in_order = get_nodes_in_order(tree.root);
	struct list_elem *elem = nodes_in_order->head;

	struct rnode *node = (struct rnode *)elem->data;
	if (strcmp("Gorilla", node->label)) {
		printf ("%s: expected node Gorilla, got %s.\n", test_name, node->label);
		return 1;
	}
	elem = elem->next;
	node = (struct rnode *) elem->data;
	if (strcmp("Pan", node->label)) {
		printf ("%s: expected node Pan, got %s.\n", test_name, node->label);
		return 1;
	}
	elem = elem->next;
	node = (struct rnode *) elem->data;
	if (strcmp("Homo", node->label)) {
		printf ("%s: expected node Homo, got %s.\n", test_name, node->label);
		return 1;
	}
	elem = elem->next;
	node = (struct rnode *) elem->data;
	if (strcmp("Hominini", node->label)) {
		printf ("%s: expected node Hominini, got %s.\n", test_name, node->label);
		return 1;
	}
	elem = elem->next;
	node = (struct rnode *) elem->data;
	if (strcmp("Homininae", node->label)) {
		printf ("%s: expected node Homininae, got %s.\n", test_name, node->label);
		return 1;
	}
	elem = elem->next;
	node = (struct rnode *) elem->data;
	if (strcmp("Hominidae", node->label)) {
		printf ("%s: expected node Hominidae, got %s.\n", test_name, node->label);
		return 1;
	}
	elem = elem->next;
	node = (struct rnode *) elem->data;
	if (strcmp("Hominoidea", node->label)) {
		printf ("%s: expected node Hominoidea, got %s.\n", test_name, node->label);
		return 1;
	}
	elem = elem->next;
	if (NULL != elem) {
		printf ("%s: list is not terminated.\n", test_name);
		return 1;
	}

	printf("%s ok.\n", test_name);
	return 0;
}

int test_children_array()
{

	const char *test_name = __func__;

	/* ((A:1,B:1.0)f:2.0,(C:1,(D:1,E:1)g:2)h:3)i; */
	struct rooted_tree tree = tree_3();
	struct hash *map = create_label2node_map(tree.nodes_in_order);
	
	struct rnode** kids;
	struct rnode *node_g = hash_get(map, "g");
	struct rnode *node_D = hash_get(map, "D");
	struct rnode *node_E = hash_get(map, "E");

	kids = children_array(node_g);

	if (kids[0] != node_D) {
		printf ("%s: expected D as first child, got %s\n",
				test_name,
				((struct rnode* )kids[0])->label);
		return 1;
	}
	if (kids[1] != node_E) {
		printf ("%s: expected E as second child, got %s\n",
				test_name,
				((struct rnode* )kids[1])->label);
		return 1;
	}

	printf ("%s ok.\n", test_name);
	return 0;
}

int main()
{
	int failures = 0;
	printf("Starting rooted node test...\n");
	failures += test_create_rnode();
	failures += test_static_rnode_vars();
	failures += test_static_rnode_vars_2();
	failures += test_create_rnode_nulllabel();
	failures += test_create_rnode_emptylabel();
	failures += test_create_rnode_nulllength();
	failures += test_create_rnode_emptylength();
	failures += test_all_children_are_leaves();
	failures += test_all_children_have_same_label();
	failures += test_get_nodes_in_order();
	failures += test_get_nodes_in_order_linear();
	failures += test_get_nodes_in_order_part_linear();
	failures += test_create_many();
	failures += test_children_array();
	if (0 == failures) {
		printf("All tests ok.\n");
	} else {
		printf("%d test(s) FAILED.\n", failures);
		return 1;
	}

	return 0;
}
