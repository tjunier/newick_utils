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

int test_mastrcat()
{
	const char *test_name = "test_mastrcat";
	
	char *result;

	char *src1 = "sbrodj";
	/* dest1 must have been obtained through malloc(), so I can't just
	 * say char *dest1 = "szohod"; */
	char *dest1 = strdup("szohod");

	result = mastrcat(dest1, src1);
	if (strcmp(result, "szohodsbrodj")) {
		printf ("%s: expected 'szohodsbrodj', got '%s'\n",
		       test_name, result);
		return 1;
	}

	char *src2 = "sbrodj";
	char *dest2 = strdup("");

	result = mastrcat(dest2, src2);
	if (strcmp(result, "sbrodj")) {
		printf ("%s: expected 'sbrodj', got '%s'\n",
		       test_name, result);
		return 1;
	}

	char *src3 = "";
	char *dest3 = strdup("szohod");

	result = mastrcat(dest3, src3);
	if (strcmp(result, "szohod")) {
		printf ("%s: expected 'szohod', got '%s'\n",
		       test_name, result);
		return 1;
	}
	
	char *src4 = "";
	char *dest4 = strdup("");

	result = mastrcat(dest4, src4);
	if (strcmp(result, "")) {
		printf ("%s: expected '', got '%s'\n",
		       test_name, result);
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
	failures += test_mastrcat();
	if (0 == failures) {
		printf("All tests ok.\n");
	} else {
		printf("%d test(s) FAILED.\n", failures);
		return 1;
	}

	return 0;
}
