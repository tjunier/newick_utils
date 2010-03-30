#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "graph_common.h"
#include "tree.h"
#include "tree_stubs.h"
#include "list.h"

int test_underscores2spaces()
{
	char *test_name = "test_underscores2spaces";
	char *test_string = strdup("_some_string_");
	char *expected = " some string ";

	underscores2spaces(test_string);	

	if (strcmp(expected, test_string) != 0) {
		printf ("%s: expected '%s', got '%s'\n", test_name,
				expected, test_string);
		return 1;
	}

	printf("%s ok.\n", test_name);
	return 0;
}

int test_remove_quotes()
{
	char *test_name = "test_remove_quotes";
	char *test_string = strdup("'some string'");
	char *expected = "some string";

	remove_quotes(test_string);	

	if (strcmp(expected, test_string) != 0) {
		printf ("%s: expected '%s', got '%s'\n", test_name,
				expected, test_string);
		return 1;
	}

	printf("%s ok.\n", test_name);
	return 0;
}

int test_prettify_labels()
{
	char *test_name = "test_prettify_labels";

	struct rooted_tree tree = tree_16();

	prettify_labels(&tree);	

	struct llist *labels = get_leaf_labels(&tree);

	char *expected, *label;

	label = (char *) labels->head->data;
	expected = "Falco peregrinus";	
	if (strcmp(expected, label) != 0) {
		printf ("%s: expected '%s', got '%s'\n", test_name,
				expected, label);
		return 1;
	}

	label = (char *) labels->head->next->data;
	expected = "Falco eleonorae";	
	if (strcmp(expected, label) != 0) {
		printf ("%s: expected '%s', got '%s'\n", test_name,
				expected, label);
		return 1;
	}

	label = (char *) labels->head->next->next->data;
	expected = "Falco rusticolus";	
	if (strcmp(expected, label) != 0) {
		printf ("%s: expected '%s', got '%s'\n", test_name,
				expected, label);
		return 1;
	}

	printf("%s ok.\n", test_name);
	return 0;
}

int test_tick_interval()
{
	char *test_name = "test_tick_interval";

	int obt, exp;

	obt = tick_interval(734);
	obt = tick_interval(100);
	obt = tick_interval(0.012);
	obt = tick_interval(52);
	obt = tick_interval(1003);
	obt = tick_interval(234);
	exp = 5;
	if (obt != exp) {
		printf ("%s: expected %d, got %d\n", test_name, exp, obt);
		return 1;
	}

	printf("%s ok.\n", test_name);
	return 0;
}

int main()
{
	int failures = 0;
	printf("Starting linking test...\n");
	failures += test_underscores2spaces();
	failures += test_remove_quotes();
	failures += test_prettify_labels();
	failures += test_tick_interval();
	if (0 == failures) {
		printf("All tests ok.\n");
	} else {
		printf("%d test(s) FAILED.\n", failures);
		return 1;
	}

	return 0;
}
