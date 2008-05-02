#include <stdio.h>
#include <string.h>

#include "nodemap.h"
#include "rnode.h"
#include "list.h"
#include "hash.h"

int test_create_and_retrieve()
{
	const char *test_name = "test_create_and_retrieve";

	struct rnode *n1, *n2, *n3;
	struct llist *node_list;
	struct hash *map;

	n1 = create_rnode("n1");
	n2 = create_rnode("n2");
	n3 = create_rnode("n3");
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

int main()
{
	int failures = 0;
	printf("Starting nodemap test...\n");
	failures += test_create_and_retrieve();
	if (0 == failures) {
		printf("All tests ok.\n");
	} else {
		printf("%d test(s) FAILED.\n", failures);
		return 1;
	}

	return 0;
}
