#include <stdio.h>

#include "node_set.h"
#include "tree_stubs.h"
#include "../src/hash.h"
#include "../src/tree.h"

int test_create()
{
	const char *test_name = "test_create";
	node_set set;

	set = create_node_set(0, 10);
	if (NULL == set) {
		printf ("%s: should not return NULL vith valid params\n", test_name);
		return 1;
	}
	set = create_node_set(-1, 10);
	if (NULL != set) {
		printf ("%s: wrong node number -1, should have returned NULL\n", test_name);
		return 1;
	}
	set = create_node_set(10, 10);
	if (NULL != set) {
		printf ("%s: wrong node number 10, should have returned NULL\n", test_name);
		return 1;
	}

	printf("%s ok.\n", test_name);
	return 0;
}

int test_is_set()
{
	const char *test_name = "test_is_set";
	int i;

	node_set set = create_node_set(0, 10);
	if (! is_set(set, 0)) {
		printf ("%s: 0 should be set\n", test_name);
		return 1;
	}
	for (i = 1; i <= 10; i++) {
		if (is_set(set, i)) {
			printf ("%s: %d should not be set.\n", test_name, i);
			return 1;
		}
	}
	set = create_node_set(9, 10);
	for (i = 0; i < 9; i++) {
		if (is_set(set, i)) {
			printf ("%s: %d should not be set.\n", test_name, i);
			return 1;
		}
	}
	if (! is_set(set, 9)) {
		printf ("%s: 9 should be set.\n", test_name);
		return 1;
	}
	printf("%s ok.\n", test_name);
	return 0;
}

int test_name2num()
{
     	const char *test_name = "test_name2num";
	struct hash *name2num;
	struct rooted_tree tree;
	int *num;

	tree = tree_2();
	build_name2num(&tree, &name2num);

	num = (int *) hash_get(name2num, "A");
	if (0 != *num) {
		printf ("%s: ord num of 'A' should be 0, not %d\n",
			  test_name, *num);
		return 1;
	}
	num = (int *) hash_get(name2num, "B");
	if (1 != *num) {
		printf ("%s: ord num of 'B' should be 1\n", test_name);
		return 1;
	}
	num = (int *) hash_get(name2num, "C");
	if (2 != *num) {
		printf ("%s: ord num of 'C' should be 2\n", test_name);
		return 1;
	}
	num = (int *) hash_get(name2num, "D");
	if (3 != *num) {
		printf ("%s: ord num of 'D' should be 3\n", test_name);
		return 1;
	}
	num = (int *) hash_get(name2num, "E");
	if (4 != *num) {
		printf ("%s: ord num of 'E' should be 4\n", test_name);
		return 1;
	}

	printf("%s ok.\n", test_name);
	return 0;
}

int test_set_union()
{
	const char *test_name = "test_set_union";
	node_set set1;
	node_set set2;
	node_set result;
	int i;

	set1 = create_node_set(3, 10);
	set2 = create_node_set(6, 10);

	result = node_set_union(set1, set2, 10);
	for (i = 0; i < 3; i++)	{
     		if (is_set(result, i)) {
			printf ("%s: %d should not be set.\n", test_name, i);
			return 1;
		}
	}
	if (! is_set(result, 3)) {
		printf ("%s: 3 should be set.\n", test_name);
		return 1;
	}
	for (i = 4; i < 6; i++)	{
     		if (is_set(result, i)) {
			printf ("%s: %d should not be set.\n", test_name, i);
			return 1;
		}
	}
	if (! is_set(result, 6)) {
		printf ("%s: 6 should be set.\n", test_name);
		return 1;
	}
	for (i = 7; i < 9; i++)	{
     		if (is_set(result, i)) {
			printf ("%s: %d should not be set.\n", test_name, i);
			return 1;
		}
	}

	return 1;
}

int main()
{
	int failures = 0;
	printf("Starting node set test...\n");
	failures += test_create();
	failures += test_is_set();
	failures += test_name2num();
	failures += test_set_union();
	if (0 == failures) {
		printf("All tests ok.\n");
	} else {
		printf("%d test(s) FAILED.\n", failures);
		return 1;
	}

	return 0;
}
