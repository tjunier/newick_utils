
#include <stdio.h>

#include "rnode.h"
#include "list.h"
#include "parser.h"
#include "newick_parser.h"
#include "tree.h"

int nwslex (void);
struct rnode *root;
struct llist *nodes_in_order;
enum parser_status_type newick_parser_status;

int test_simple()
{
	const char *test_name = "test_simple";

	/* Note: this is a valid Newick string, but since we're checking the
	 * scanner, it doesn't have to be - see test_garbled()  */
	char *newick = "((A,B),C);";

	newick_scanner_set_string_input(newick);

	struct rooted_tree *tree = parse_tree();
	char *obt = to_newick(tree->root);
	if (strcmp(obt, newick) != 0) {
		printf ("%s: expected '%s', got '%s'\n", test_name,
				newick, obt);
		return 1;
	}

	printf ("%s: ok.\n", test_name);
	return 0;
}

int main()
{
	int failures = 0;
	printf("Starting newick parser test...\n");
	failures += test_simple();
	if (0 == failures) {
		printf("All tests ok.\n");
	} else {
		printf("%d test(s) FAILED.\n", failures);
		return 1;
	}

	return 0;
}
