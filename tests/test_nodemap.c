#include <stdio.h>
#include <string.h>

#include "nodemap.h"
#include "rnode.h"
#include "list.h"
#include "hash.h"
#include "tree.h"
#include "tree_stubs.h"

int test_create_label2node_map()
{
	const char *test_name = "test_create_label2node_map";

	struct rnode *n1, *n2, *n3;
	struct llist *node_list;
	struct hash *map;

	n1 = create_rnode("n1", "");
	n2 = create_rnode("n2", "");
	n3 = create_rnode("n3", "");
	node_list = create_llist();
	append_element(node_list, n1);
	append_element(node_list, n2);
	append_element(node_list, n3);
	map = create_label2node_map(node_list);

	if (NULL == map) {
		printf ("%s: map must not be NULL.\n", test_name);
		return 1;
	}
	if (NULL != hash_get(map, "not there")) {
		printf ("%s: inexistent label should return NULL.\n",
				test_name);
		return 1;
	}
	if (n1 != hash_get(map, "n1")) {
		printf ("%s: node with label 'n1' should be %p, not %p.\n",
			test_name, n1, hash_get(map, "n1")); 
		return 1;
	}
	if (n2 != hash_get(map, "n2")) {
		printf ("%s: node with label 'n2' should be %p, not %p.\n",
			test_name, n2, hash_get(map, "n2")); 
		return 1;
	}
	if (n3 != hash_get(map, "n3")) {
		printf ("%s: node with label 'n3' should be %p, not %p.\n",
			test_name, n3, hash_get(map, "n3")); 
		return 1;
	}


	printf("%s ok.\n", test_name);
	return 0;
}

int test_create_label2node_list_map()
{
	const char *test_name = "test_create_label2node_list_map";

	struct rnode *a1, *a2, *a3, *b1, *b2, *c1, *d1, *d2;
	struct llist *node_list;
	struct hash *map;
	struct llist *nodes_of_label;
	struct list_elem *el;

	a1 = create_rnode("a", "");
	a2 = create_rnode("a", "");
	a3 = create_rnode("a", "");
	b1 = create_rnode("b", "");
	b2 = create_rnode("b", "");
	c1 = create_rnode("c", "");
	d1 = create_rnode("d", "");
	d2 = create_rnode("d", "");

	node_list = create_llist();
	/* The order in which the nodes appear is unimportant, but it should be
	 * preserved among nodes of the same label - which is why we test them
	 * below in the same order (in a given label) */
	append_element(node_list, d2);
	append_element(node_list, b2);
	append_element(node_list, a2);
	append_element(node_list, a3);
	append_element(node_list, c1);
	append_element(node_list, d1);
	append_element(node_list, b1);
	append_element(node_list, a1);

	map = create_label2node_list_map(node_list);

	if (NULL == map) {
		printf ("%s: map must not be NULL.\n", test_name);
		return 1;
	}
	if (NULL != hash_get(map, "not there")) {
		printf ("%s: inexistent label should return NULL.\n",
				test_name);
		return 1;
	}

	nodes_of_label = hash_get(map, "a");
	if (NULL == nodes_of_label) {
		printf ("%s: there should be a list of nodes with label 'a'.\n",
				test_name);
		return 1;
	}
	if (nodes_of_label->count != 3) {
		printf ("%s: list of nodes with label 'a' should"
				" have length 3\n", test_name);
		return 1;
	}
	el = nodes_of_label->head;
	if (el->data != a2) {
		printf ("%s: expected node a2\n", test_name);
		return 1;
	}
	el = el->next;
	if (el->data != a3) {
		printf ("%s: expected node a3\n", test_name);
		return 1;
	}
	el = el->next;
	if (el->data != a1) {
		printf ("%s: expected node a1\n", test_name);
		return 1;
	}
	el = el->next;
	if (NULL != el) {
		printf ("%s: list of nodes with label 'a' is not terminated.\n",test_name);
		return 1;
	}


	nodes_of_label = hash_get(map, "b");
	if (NULL == nodes_of_label) {
		printf ("%s: there should be a list of nodes with label 'b'.\n",
				test_name);
		return 1;
	}
	if (nodes_of_label->count != 2) {
		printf ("%s: list of nodes with label 'b' should"
				" have length 2\n", test_name);
		return 1;
	}
	el = nodes_of_label->head;
	if (el->data != b2) {
		printf ("%s: expected node b2\n", test_name);
		return 1;
	}
	el = el->next;
	if (el->data != b1) {
		printf ("%s: expected node b1\n", test_name);
		return 1;
	}
	el = el->next;
	if (NULL != el) {
		printf ("%s: list of nodes with label 'b' is not terminated.\n",test_name);
		return 1;
	}

	nodes_of_label = hash_get(map, "c");
	if (NULL == nodes_of_label) {
		printf ("%s: there should be a list of nodes with label 'c'.\n",
				test_name);
		return 1;
	}
	if (nodes_of_label->count != 1) {
		printf ("%s: list of nodes with label 'c' should"
				" have length 1\n", test_name);
		return 1;
	}
	el = nodes_of_label->head;
	if (el->data != c1) {
		printf ("%s: expected node c1\n", test_name);
		return 1;
	}
	el = el->next;
	if (NULL != el) {
		printf ("%s: list of nodes with label 'b' is not terminated.\n",test_name);
		return 1;
	}

	nodes_of_label = hash_get(map, "d");
	if (NULL == nodes_of_label) {
		printf ("%s: there should be a list of nodes with label 'd'.\n",
				test_name);
		return 1;
	}
	if (nodes_of_label->count != 2) {
		printf ("%s: list of nodes with label 'd' should"
				" have length 2\n", test_name);
		return 1;
	}
	el = nodes_of_label->head;
	if (el->data != d2) {
		printf ("%s: expected node d2\n", test_name);
		return 1;
	}
	el = el->next;
	if (el->data != d1) {
		printf ("%s: expected node d1\n", test_name);
		return 1;
	}
	el = el->next;
	if (NULL != el) {
		printf ("%s: list of nodes with label 'b' is not terminated.\n",test_name);
		return 1;
	}

	printf("%s ok.\n", test_name);
	return 0;
}

int test_get_leaf_label_map_from_node()
{

	const char *test_name = "test_get_leaf_label_map_from_node";

	/* ((A:1,B:1.0)f:2.0,(C:1,(D:1,E:1)g:2)h:3)i; */
	struct rooted_tree tree = tree_3();
	struct hash *map = get_leaf_label_map_from_node(tree.root);

	if (5 != map->count) {
		printf ("%s: expected hash count of 5, got %d.\n", test_name, map->count);
		return 1;
	}
	if (NULL == hash_get(map, "A")) {
		printf ("%s: leaf A not found in map.\n", test_name);
		return 1;
	}
	if (NULL == hash_get(map, "B")) {
		printf ("%s: leaf B not found in map.\n", test_name);
		return 1;
	}
	if (NULL == hash_get(map, "C")) {
		printf ("%s: leaf C not found in map.\n", test_name);
		return 1;
	}
	if (NULL == hash_get(map, "D")) {
		printf ("%s: leaf D not found in map.\n", test_name);
		return 1;
	}
	if (NULL == hash_get(map, "E")) {
		printf ("%s: leaf E not found in map.\n", test_name);
		return 1;
	}

	printf ("%s ok.\n", test_name);
	return 0;
}

int main()
{
	int failures = 0;
	printf("Starting nodemap test...\n");
	failures += test_create_label2node_map();
	failures += test_create_label2node_list_map();
	failures += test_get_leaf_label_map_from_node();
	if (0 == failures) {
		printf("All tests ok.\n");
	} else {
		printf("%d test(s) FAILED.\n", failures);
		return 1;
	}

	return 0;
}
