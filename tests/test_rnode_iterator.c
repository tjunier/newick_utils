#include <stdio.h>
#include <string.h>

#include "rnode_iterator.h"

int test_simple()
{
	const char *test_name = "test_simple";

	/* TODO: remove - just commented this out so it compiles.
	 *
	struct rooted_tree tree = tree_3();
	struct rnode_iterator *it = create_rnode_iterator(tree.root);

	printf ("%s: expected '%s', got '%s' (line %d)\n", test_name, blank, cp->lines[i], i);

	printf("%s ok.\n", test_name);
	*/
	return 0;
}

int main()
{
	int failures = 0;
	printf("Starting canvas test...\n");
	failures += test_simple();
	if (0 == failures) {
		printf("All tests ok.\n");
	} else {
		printf("%d test(s) FAILED.\n", failures);
		return 1;
	}

	return 0;
}
