#include <stdio.h>
#include <string.h>

char *transform_ornaments(const char *, double, double, double);

unsigned int test_transform_ornaments()
{
	const char *test_name = __func__;

	const char *case_1 = "<circle r='2'/>";
	const char *exp_1 = "<circle r=\"2\" transform=\"translate(10,10) rotate(45)\"/>";
	const char *out_1 = transform_ornaments(case_1, 45.0, 10.0, 10.0);

	if (strcmp(out_1, exp_1) != 0) {
		printf ("%s: expected '%s', got '%s'\n", test_name, exp_1,
				out_1);
		return 1;
	}

	const char *case_2 = "<circle r='3' transform='scale(2.1)'/>";
	const char *exp_2 = "<circle r=\"3\" transform=\"scale(2.1), translate(10,10)\"/>";
	const char *out_2 = transform_ornaments(case_2, 45.0, 10.0, 10.0);

	if (strcmp(out_2, exp_2) != 0) {
		printf ("%s: expected '%s', got '%s'\n", test_name, exp_2,
				out_2);
		return 1;
	}

	const char *case_3 = "<rect y='-3' height='6' width='9'/>";
	const char *exp_3 = "<rect y=\"-3\" height=\"6\" width=\"9\" transform=\"rotate(45), translate(10,10)\"/>";
	const char *out_3 = transform_ornaments(case_3, 45.0, 10.0, 10.0);

	if (strcmp(out_3, exp_3) != 0) {
		printf ("%s: expected '%s', got '%s'\n", test_name, exp_3,
				out_3);
		return 1;
	}

	/*
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
	*/

	printf("%s ok.\n", test_name);

	return 0;
}

int main()
{
	int failures = 0;
	printf("Starting svg_graph_radial test...\n");
	failures += test_transform_ornaments();
	if (0 == failures) {
		printf("All tests ok.\n");
	} else {
		printf("%d test(s) FAILED.\n", failures);
		return 1;
	}

	return 0;
}
