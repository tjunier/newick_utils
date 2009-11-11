#include <stdio.h>
#include <string.h>

#include "error.h"

int test_unset()
{
	const char *test_name = "test_unset";
	enum error_codes result;
	result = get_last_error_code(); /* not set... */
	if (result != ERR_UNSET) {
		printf ("%s: expected ERR_UNSET (%d), got %d.\n",
				test_name, ERR_UNSET, result);
		return 1;
	}

	printf("%s ok.\n", test_name);
	return 0;
}

int test_set_get()
{
	const char *test_name = "test_set_get";
	enum error_codes result;
	set_last_error_code(ERR_NOMEM);
	set_last_error_code(ERR_NW_SYNTAX);
	result = get_last_error_code();
	if (result != ERR_NW_SYNTAX) {
		printf ("%s: expected ERR_NW_SYNTAX (%d), got %d.\n",
				test_name, ERR_NW_SYNTAX, result);
		return 1;
	}

	printf("%s ok.\n", test_name);
	return 0;
}

int test_get_msg()
{
	const char *test_name = "test_get_msg";

	char *msg;

	set_last_error_code(ERR_NOMEM);
	msg = get_last_error_message();
	if (strcmp(msg, "Out of memory") != 0) {
		printf ("%s: expected 'Out of memory', got '%s'\n",
				test_name, msg);
		return 1;
	}
	set_last_error_code(ERR_NW_SYNTAX);
	msg = get_last_error_message();
	if (strcmp(msg, "Syntax error in Newick") != 0) {
		printf ("%s: expected 'Syntax error in Newick', got '%s'\n",
				test_name, msg);
		return 1;
	}

	printf("%s ok.\n", test_name);
	return 0;
}

int main()
{
	int failures = 0;
	printf("Starting canvas test...\n");
	failures += test_unset();
	failures += test_set_get();
	failures += test_get_msg();
	if (0 == failures) {
		printf("All tests ok.\n");
	} else {
		printf("%d test(s) FAILED.\n", failures);
		return 1;
	}

	return 0;
}
