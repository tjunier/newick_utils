#include <stdio.h>
#include <string.h>

#include "rnode.h"

int test_create_rnode()
{
	const char *test_name = "test_create_rnode";
	struct rnode *nodep;
	char *label = "test";
	char *length = "2.0456";
	nodep = create_rnode(label,length);
	if (0 != strcmp(nodep->label, label)) {
		printf("%s: expected label '%s' (got '%s')\n",
				test_name, label, nodep->label);
		return 1;
	}
	if (0 != strcmp(nodep->edge_length_as_string, length)) {
		printf("%s: expected length '%s', got '%s'\n",
			test_name, length, nodep->edge_length_as_string);
		return 1;
	}
	if (NULL == nodep->children) {
		printf("%s: children list should not be NULL.\n", test_name);
		return 1;
	}
	if (NULL != nodep->parent) {
		printf ("%s: parent should be NULL.\n", test_name);
		return 1;
	}
	printf("%s ok.\n", test_name);
	return 0;
}

int test_create_rnode_nulllabel()
{
	const char *test_name = "test_create_rnode_nulllabel";
	struct rnode *nodep;
	nodep = create_rnode(NULL,"");
	if (0 != strcmp(nodep->label, "")) {
		printf("%s: expected empty label (""), got '%s'\n",
				test_name, nodep->label);
		return 1;
	}
	if (0 != strcmp(nodep->edge_length_as_string, "")) {
		printf("%s: expected empty length, got '%s'\n",
			test_name, nodep->edge_length_as_string);
		return 1;
	}
	if (NULL == nodep->children) {
		printf("%s: children list should not be NULL.\n", test_name);
		return 1;
	}
	if (NULL != nodep->parent) {
		printf ("%s: parent node should be NULL.\n", test_name);
		return 1;
	}
	printf("%s ok.\n", test_name);
	return 0;
}

int test_create_rnode_emptylabel()
{
	const char *test_name = "test_create_rnode_emptylabel";
	struct rnode *nodep;
	nodep = create_rnode("","");
	if (0 != strcmp(nodep->label, "")) {
		printf("%s: expected empty label (""), got '%s'\n",
				test_name, nodep->label);
		return 1;
	}
	if (0 != strcmp(nodep->edge_length_as_string, "")) {
		printf("%s: expected empty length, got '%s'\n",
			test_name, nodep->edge_length_as_string);
		return 1;
	}
	if (NULL == nodep->children) {
		printf("%s: children list should not be NULL.\n", test_name);
		return 1;
	}
	if (NULL != nodep->parent) {
		printf ("%s: parent node should be NULL.\n", test_name);
		return 1;
	}
	printf("%s ok.\n", test_name);
	return 0;
}

int test_create_rnode_nulllength()
{
	const char *test_name = "test_create_rnode_nulllength";
	struct rnode *nodep;
	char *label = "test";
	nodep = create_rnode(label,NULL);
	if (0 != strcmp(nodep->label, label)) {
		printf("%s: expected label '%s' (got '%s')\n",
				test_name, label, nodep->label);
		return 1;
	}
	if (0 != strcmp(nodep->edge_length_as_string, "")) {
		printf("%s: expected empty length, got '%s'\n",
			test_name, nodep->edge_length_as_string);
		return 1;
	}
	if (NULL == nodep->children) {
		printf("%s: children list should not be NULL.\n", test_name);
		return 1;
	}
	if (NULL != nodep->parent) {
		printf ("%s: parent should be NULL.\n", test_name);
		return 1;
	}
	printf("%s ok.\n", test_name);
	return 0;
}

int test_create_rnode_emptylength()
{
	const char *test_name = "test_create_rnode_emptylength";
	struct rnode *nodep;
	char *label = "test";
	nodep = create_rnode(label,"");
	if (0 != strcmp(nodep->label, label)) {
		printf("%s: expected label '%s' (got '%s')\n",
				test_name, label, nodep->label);
		return 1;
	}
	if (0 != strcmp(nodep->edge_length_as_string, "")) {
		printf("%s: expected empty length, got '%s'\n",
			test_name, nodep->edge_length_as_string);
		return 1;
	}
	if (NULL == nodep->children) {
		printf("%s: children list should not be NULL.\n", test_name);
		return 1;
	}
	if (NULL != nodep->parent) {
		printf ("%s: parent should be NULL.\n", test_name);
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
		nodep = create_rnode("test", "2.34");
	}
	printf ("test_create_many Ok (created %d).\n", num);
	return 0; 	/* crashes on failure */
}

int main()
{
	int failures = 0;
	printf("Starting rooted node test...\n");
	failures += test_create_rnode();
	failures += test_create_rnode_nulllabel();
	failures += test_create_rnode_emptylabel();
	failures += test_create_rnode_nulllength();
	failures += test_create_rnode_emptylength();
	failures += test_create_many();
	if (0 == failures) {
		printf("All tests ok.\n");
	} else {
		printf("%d test(s) FAILED.\n", failures);
		return 1;
	}

	return 0;
}
