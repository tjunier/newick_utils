#include <stdio.h>
#include <string.h>

#include "redge.h"

int test_create()
{
	const char *test_name = "test_create";
	struct redge *edgep;
	char *length = "12.345";
	edgep = create_redge(length);
	if (strcmp(edgep->length_as_string, length) != 0) {
		printf("%s: expected lenth string '%s', got '%s'\n",
				test_name, edgep->length_as_string,
				length);
		return 1;
	}
	printf("%s Ok.\n", test_name);
	return 0;
}

int test_create_null_length()
{
	const char *test_name = "test_create_null_length";
	struct redge *edgep;
	edgep = create_redge(NULL);
	if (0 != strcmp("", edgep->length_as_string)) {
		printf("%s: expected "" lenth string, got '%s'\n",
				test_name, edgep->length_as_string);
		return 1;
	}
	printf("%s Ok.\n", test_name);
	return 0;
}

int test_create_many()
{
	int i;
	int num = 100000;
	for (i = 0; i < num; i++) {
		struct redge *edgep;
		edgep = create_redge("test");
	}
	printf ("test_create_many Ok (created %d).\n", num);
	return 0;	/* crashes on failure... */
}

int main()
{
	int failures = 0;
	printf("Starting rooted edge test...\n");
	failures += test_create();
	failures += test_create_null_length();
	failures += test_create_many();
	if (0 == failures) {
		printf("All tests ok.\n");
	} else {
		printf("%d test(s) FAILED.\n", failures);
		return 1;
	}

	return 0;
}
