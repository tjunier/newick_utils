#include <stdlib.h>
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

int test_append()
{
	char *test_name = "test_append";
	char *s1;
	char *s2 = "urumq";

	s1 = malloc(6 * sizeof(char));
	strcpy(s1, "brobd");

	s1 = append_to(s1, s2);

	if (strcmp(s1, "brobdurumq") != 0) {
		printf ("%s: expected 'brobdurumq', got %s\n",
				test_name, s1);
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
	failures += test_append();
	if (0 == failures) {
		printf("All tests ok.\n");
	} else {
		printf("%d test(s) FAILED.\n", failures);
		return 1;
	}

	return 0;
}
