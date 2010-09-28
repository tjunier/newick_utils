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

int main()
{
	int failures = 0;
	printf("Starting XML utilities test...\n");
	failures += test_escape_predefined_character_entities();
	if (0 == failures) {
		printf("All tests ok.\n");
	} else {
		printf("%d test(s) FAILED.\n", failures);
		return 1;
	}

	return 0;
}
