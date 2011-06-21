#include <stdio.h>
#include <string.h>

#include "masprintf.h"

int test_masprintf()
{
	const char *test_name = "test_masprintf";

	char *result = masprintf ("%s", "glops");
	if (strcmp(result, "glops") != 0) {
		printf ("%s: expected 'glops', got '%s'\n", test_name,
				result);
		return 1;
	}

	printf ("%s: ok.\n", test_name);
	return 0;
}

int main()
{
	int failures = 0;
	printf("Starting masprintf test...\n");
	failures += test_masprintf();
	if (0 == failures) {
		printf("All tests ok.\n");
	} else {
		printf("%d test(s) FAILED.\n", failures);
		return 1;
	}

	return 0;
}
