#include <stdio.h>
#include <string.h>

#include "xml_utils.h"

int test_escape_predefined_character_entities()
{
	const char *test_name = "test_escape_predefined_character_entities";

	const char *s1 = "no entities";
	if (strcmp(escape_predefined_character_entities(s1), s1) != 0) {
		printf ("%s: expected '%s', got '%s'\n", test_name, s1, 
			escape_predefined_character_entities(s1));
			return 1;
	}

	const char *s2 = "an & ampersand";
	if (strcmp(escape_predefined_character_entities(s2), 
				"an &amp; ampersand") != 0) {
		printf ("%s: expected 'an &amp; ampersand', got '%s'\n", test_name, escape_predefined_character_entities(s2));
			return 1;
	}

	const char *s3 = "& < > ' \"";
	if (strcmp(escape_predefined_character_entities(s3), 
				"&amp; &lt; &gt; &apos; &quot;") != 0) {
		printf ("%s: expected '&amp; &lt; &gt; &apos; &quot;', got '%s'\n", test_name, escape_predefined_character_entities(s3));
			return 1;
	}

	const char *s4 = "''''''''''";
	const char *exp = "&apos;&apos;&apos;&apos;&apos;&apos;&apos;&apos;&apos;&apos;";
	if (strcmp(escape_predefined_character_entities(s4), exp) != 0) {
		printf ("%s: expected '%s', got '%s'\n", test_name, exp, 
			escape_predefined_character_entities(s4));
			return 1;
	}
	printf("%s ok.\n", test_name);
	return 0;
}

int test_change_svg_x_attr_sign()
{
	const char *test_name = __func__;

	const char *case_1 = "<text>a text</text>";
	const char *exp_1 = "<text>a text</text>";
	const char *out_1 = change_svg_x_attr_sign(case_1);

	if (strcmp(out_1, exp_1) != 0) {
		printf ("%s: expected '%s', got '%s'\n", test_name, exp_1,
				out_1);
		return 1;
	}

	const char *case_2 = "<text x='10'>a text with an x-attribute</text>";
	const char *exp_2 = "<text x=\"-10\">a text with an x-attribute</text>";
	const char *out_2 = change_svg_x_attr_sign(case_2);

	if (strcmp(out_2, exp_2) != 0) {
		printf ("%s: expected '%s', got '%s'\n", test_name, exp_2,
				out_2);
		return 1;
	}

	printf("%s ok.\n", test_name);
	return 0;
}

int main()
{
	int failures = 0;
	printf("Starting XML utilities test...\n");
	failures += test_escape_predefined_character_entities();
	failures += test_change_svg_x_attr_sign();
	if (0 == failures) {
		printf("All tests ok.\n");
	} else {
		printf("%d test(s) FAILED.\n", failures);
		return 1;
	}

	return 0;
}
