#include <stdio.h>
#include <string.h>

#include "svg_graph_radial.h"

unsigned int test_transform_ornaments()
{
	const char *test_name = __func__;

	const char *case_1 = "<text>a text</text>";
	const char *exp_1 = "<text>a text</text>";
	const char *out_1 = transform_ornaments(case_1);

	if (strcmp(out_1, exp_1) != 0) {
		printf ("%s: expected '%s', got '%s'\n", test_name, exp_1,
				out_1);
		return 1;
	}

	const char *case_2 = "<text x='10'>a text with an x-attribute</text>";
	const char *exp_2 = "<text x=\"-10\">a text with an x-attribute</text>";
	const char *out_2 = transform_ornaments(case_2);

	if (strcmp(out_2, exp_2) != 0) {
		printf ("%s: expected '%s', got '%s'\n", test_name, exp_2,
				out_2);
		return 1;
	}

	const char *case_3 = "<text x='-10'>a text with an x-attribute</text>";
	const char *exp_3 = "<text x=\"10\">a text with an x-attribute</text>";
	const char *out_3 = transform_ornaments(case_3);

	if (strcmp(out_3, exp_3) != 0) {
		printf ("%s: expected '%s', got '%s'\n", test_name, exp_3,
				out_3);
		return 1;
	}

	const char *case_4 = "<text x='-10.23'>a text with an x-attribute</text>";
	const char *exp_4 = "<text x=\"10.23\">a text with an x-attribute</text>";
	const char *out_4 = transform_ornaments(case_4);

	if (strcmp(out_4, exp_4) != 0) {
		printf ("%s: expected '%s', got '%s'\n", test_name, exp_4,
				out_4);
		return 1;
	}

	const char *case_5 = "<circle cx='-10.23' cy='0.12' r='6'/>";
	const char *exp_5 = "<circle cx=\"10.23\" cy=\"0.12\" r=\"6\"/>";
	const char *out_5 = transform_ornaments(case_5);

	if (strcmp(out_5, exp_5) != 0) {
		printf ("%s: expected '%s', got '%s'\n", test_name, exp_5,
				out_5);
		return 1;
	}

	const char *case_6 = "<image x='10' y='-50' width='100' height='100' xlink:href='100px-square-Giraffe.png'/>";
	const char *exp_6 = "<image x=\"-10\" y=\"-50\" width=\"100\" height=\"100\" xlink:href=\"100px-square-Giraffe.png\"/>";
	const char *out_6 = transform_ornaments(case_6);

	if (strcmp(out_6, exp_6) != 0) {
		printf ("%s: expected '%s', got '%s'\n", test_name, exp_6,
				out_6);
		return 1;
	}

	printf("%s ok.\n", test_name);

	return 0;
}

int main()
{
	int failures = 0;
	printf("Starting canvas test...\n");
	failures += test_transform_ornaments();
	if (0 == failures) {
		printf("All tests ok.\n");
	} else {
		printf("%d test(s) FAILED.\n", failures);
		return 1;
	}

	return 0;
}
