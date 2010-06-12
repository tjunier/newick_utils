#include <stdio.h>
#include <string.h>

#include "rnode.h"
#include "list.h"
#include "parser.h"
#include "newick_parser.h"
#include "tree.h"
#include "to_newick.h"

int nwslex (void);
struct rnode *root;
struct llist *nodes_in_order;
enum parser_status_type newick_parser_status;

/* NOTE: we can use to_newick() to check the parser's output because this
 * function is independently tested on trees constructed without the parser
 * (see test_to_newick) */

int check_tree(char *test_name, char *newick)
{
	newick_scanner_set_string_input(newick);

	struct rooted_tree *tree = parse_tree();
	char *obt = to_newick(tree->root);
	if (strcmp(obt, newick) != 0) {
		printf ("%s: expected '%s', got '%s'\n", test_name,
				newick, obt);
		return 1;
	}

	return 0;
}


int test_simple()
{
	char *test_name = "test_simple";

	char *newick = "((A,B),C);";

	int result = check_tree(test_name, newick);

	if (0 == result) {
		printf ("%s: ok.\n", test_name);
		return 0;
	} else
		return 1;
}

/* The test__jf_* tests all test trees from Felsenstein's description of Newick
 * at http://evolution.genetics.washington.edu/phylip/newicktree.html, which is
 * the closest thing to an official description. */

int test_jf_1()
{

	char *test_name = "test_jf_1";

	char *newick = "(B,(A,C,E),D);";

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
	failures += test_jf_1();
	if (0 == failures) {
		printf("All tests ok.\n");
	} else {
		printf("%d test(s) FAILED.\n", failures);
		return 1;
	}

	return 0;
}
