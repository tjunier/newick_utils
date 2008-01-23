#include <stdio.h>

#include "node_set.h"

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

int main()
{
	int failures = 0;
	printf("Starting node set test...\n");
	failures += test_create();
	failures += test_is_set();
	if (0 == failures) {
		printf("All tests ok.\n");
	} else {
		printf("%d test(s) FAILED.\n", failures);
		return 1;
	}

	return 0;
}
