#include <string.h>
#include <stdio.h>

#include "concat.h"

int test_concat()
{
	char *test_name = "test_concat";
	char *s1 = "brobd";
	char *s2 = "urumq";

	if (strcmp(concat(s1, s2), "brobdurumq") != 0) {
		printf ("%s: expected 'brobdurumq', got %s\n",
				test_name, concat(s1, s2));
		return 1;
	}

	printf ("%s ok.\n", test_name);
	return 0;
}

int main()
{
	int failures = 0;
	printf("Starting concat test...\n");
	failures += test_concat();
	if (0 == failures) {
		printf("All tests ok.\n");
	} else {
		printf("%d test(s) FAILED.\n", failures);
		return 1;
	}

	return 0;
}
