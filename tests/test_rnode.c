#include <stdio.h>
#include <string.h>

#include "rnode.h"

int test_create_rnode()
{
	const char *test_name = "test_create";
	struct rnode *nodep;
	char *label = "test";
	nodep = create_rnode(label);
	if (0 != strcmp(nodep->label, label)) {
		printf("%s: expected label '%s' (got '%s')\n",
				test_name, label, nodep->label);
		return 1;
	}
	if (NULL == nodep->children) {
		printf("%s: children list should not be NULL.\n", test_name);
		return 1;
	}
	if (NULL != nodep->parent_edge) {
		printf ("%s: parent node should be NULL.\n", test_name);
		return 1;
	}
	printf("%s ok.\n", test_name);
	return 0;
}

int test_create_rnode_null()
{
	const char *test_name = "test_create_null";
	struct rnode *nodep;
	nodep = create_rnode(NULL);
	if (0 != strcmp(nodep->label, "")) {
		printf("%s: expected empty label (""), got '%s'\n",
				test_name, nodep->label);
		return 1;
	}
	if (NULL == nodep->children) {
		printf("%s: children list should not be NULL.\n", test_name);
		return 1;
	}
	if (NULL != nodep->parent_edge) {
		printf ("%s: parent node should be NULL.\n", test_name);
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
		struct rnode *nodep;
		nodep = create_rnode("test");
	}
	printf ("test_create_many Ok (created %d).\n", num);
	return 0; 	/* crashes on failure */
}

int main()
{
	int failures = 0;
	printf("Starting rooted node test...\n");
	failures += test_create_rnode();
	failures += test_create_rnode_null();
	failures += test_create_many();
	if (0 == failures) {
		printf("All tests ok.\n");
	} else {
		printf("%d test(s) FAILED.\n", failures);
		return 1;
	}

	return 0;
}
