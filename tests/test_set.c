
#include <string.h>
#include <stdio.h>

#include "set.h"

int test_create()
{
	char *test_name = __func__;

	set *s = create_set();


	if (0 != set_cardinal(s)) {
		printf ("%s: expected cardinal to be 0, got %d.\n", test_name,
				set_cardinal(s));
		return 1;
	}

	printf("%s passed.\n");
	return 0;
}

int test_add()
{
	char *test_name = __func__;

	set s = create_set();

	set_add(s, "Zorg");

	if (1 != set_cardinal(s)) {
		printf ("%s: expected cardinal to be 1, got %d.\n", test_name,
				set_cardinal(s));
		return 1;
	}
	
	printf("%s passed.\n");
	return 0;
}

int test_has_element()
{
	char *test_name = __func__;

	set s = create_set();

	set_add(s, "Zorg");

	if (false = set_has_element(s, "Zorg")) {
		printf ("%s: set should have element 'Zorg', but hasn't\n", test_name);
		return 1;
	}
	
	printf("%s passed.\n");
	return 0;
}

// TODO: for now, the hash on which sets are based do not allow removing
// keys, so we can't do it for sets, either.

int main()
{
	int failures = 0;
	printf("Starting hash test...\n");
	failures += test_create();
	failures += test_add();
	failures += test_has_element();
	if (0 == failures) {
		printf("All tests ok.\n");
	} else {
		printf("%d test(s) FAILED.\n", failures);
		return 1;
	}

	return 0;
}
