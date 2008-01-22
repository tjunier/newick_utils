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
		printf ("%s: wrong params, should have returned NULL\n", test_name);
		return 1;
	}
	set = create_node_set(10, 10);
	if (NULL != set) {
		printf ("%s: wrong params, should have returned NULL\n", test_name);
		return 1;
	}
}

int test_is_set()
{
	const char *test_name = "test_create";

	node_set set = create_node_set(0, 10);
	if (! is_set(set, 0)) {
		printf ("%s: wrong params, should have returned NULL\n", test_name);
		return 1;
	}
	// test 1-9 false
	// test 10 false
	printf("%s ok.\n", test_name);
	return 0;
}

int main()
{
	int failures = 0;
	printf("Starting canvas test...\n");
	failures += test_create();
	if (0 == failures) {
		printf("All tests ok.\n");
	} else {
		printf("%d test(s) FAILED.\n", failures);
		return 1;
	}

	return 0;
}
