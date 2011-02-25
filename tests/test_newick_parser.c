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
void newick_scanner_set_string_input(char *);

/* NOTE: we can use to_newick() to check the parser's output because this
 * function is independently tested on trees constructed without the parser
 * (see test_to_newick) */
/* NOTE HOWEVER: one of the test cases below (test_jf) actually uncovered a bug
 * in to_newick()... This should serve as a lesson :-) */

int check_tree(const char *test_name, char *newick)
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

/* This tests all test trees from Felsenstein's description of Newick
 * at http://evolution.genetics.washington.edu/phylip/newicktree.html, which is
 * the closest thing to an official description. */

int test_jf()
{

	const char *test_name = __func__;

	int failures = 0;

	failures += check_tree(test_name, "(B,(A,C,E),D);");
	failures += check_tree(test_name, "(,(,,),);");
	failures += check_tree(test_name, "(B:6.0,(A:5.0,C:3.0,E:4.0):5.0,D:11.0);");
	failures += check_tree(test_name, "(B:6.0,(A:5.0,C:3.0,E:4.0)Ancestor1:5.0,D:11.0);");
	failures += check_tree(test_name, "((raccoon:19.19959,bear:6.80041):0.84600,((sea_lion:11.99700,seal:12.00300):7.52973,((monkey:100.85930,cat:47.14069):20.59201,weasel:18.87953):2.09460):3.87382,dog:25.46154);");
	failures += check_tree(test_name, "(Bovine:0.69395,(Gibbon:0.36079,(Orang:0.33636,(Gorilla:0.17147,(Chimp:0.19268,Human:0.11927):0.08386):0.06124):0.15057):0.54939,Mouse:1.21460):0.10;");
	failures += check_tree(test_name, "(Bovine:0.69395,(Hylobates:0.36079,(Pongo:0.33636,(G._Gorilla:0.17147,(P._paniscus:0.19268,H._sapiens:0.11927):0.08386):0.06124):0.15057):0.54939,Rodent:1.21460);");
	failures += check_tree(test_name, "A;");
	failures += check_tree(test_name, "((A,B),(C,D));");
	failures += check_tree(test_name, "(Alpha,Beta,Gamma,Delta,,Epsilon,,,);");
	failures += check_tree(test_name, "(B,(A,D),C);");
	failures += check_tree(test_name, "(A,(B,C),D);");
	failures += check_tree(test_name, "((A,D),(C,B));");

	if (failures == 0)
		printf ("%s: ok.\n", test_name);
	return failures;
}

int main()
{
	int failures = 0;
	printf("Starting newick parser test...\n");
	failures += test_simple();
	failures += test_jf();
	if (0 == failures) {
		printf("All tests ok.\n");
	} else {
		printf("%d test(s) FAILED.\n", failures);
		return 1;
	}

	return 0;
}
