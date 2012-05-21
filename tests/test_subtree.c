#include <stdio.h>

#include "rnode.h"
#include "list.h"
#include "link.h"
#include "subtree.h"

int test_is_monophyletic()
{
	const char *test_name = __func__;

	struct rnode *node_a = create_rnode("a", NULL);
	struct rnode *node_b = create_rnode("b", NULL);
	struct rnode *node_c = create_rnode("c", NULL);
	struct rnode *node_d = create_rnode("", NULL);
	struct rnode *node_e = create_rnode("", NULL);
	struct rnode *node_f = create_rnode("", NULL);
	struct llist *descendants = create_llist();
	enum monophyly result;

	/* (a,(b,c)); */
	add_child(node_d, node_b);
	add_child(node_d, node_c);
	add_child(node_e, node_a);
	add_child(node_e, node_d);

	append_element(descendants, node_c);

	// TODO: test w/ empty list

	result = is_monophyletic(descendants, node_c);
	if (MONOPH_TRUE != result) {
		printf ("%s: c should be monophyletic in (c)\n", test_name);
		return 1;
	}

	result = is_monophyletic(descendants, node_d);
	if (MONOPH_FALSE != result) {
		printf ("%s: c should NOT be monophyletic in (c,b)\n",
				test_name);
		return 1;
	}

	append_element(descendants, node_b);
	result = is_monophyletic(descendants, node_d);
	if (MONOPH_TRUE != result) {
		printf ("%s: c,b should be monophyletic in (c,b)\n",
				test_name);
		return 1;
	}

	append_element(descendants, node_a);
	result = is_monophyletic(descendants, node_d);
	if (MONOPH_FALSE != result) {
		printf ("%s: a,c,b should NOT be monophyletic in (c,b)\n",
				test_name);
		return 1;
	}

	result = is_monophyletic(descendants, node_e);
	if (MONOPH_TRUE != result) {
		printf ("%s: a,c,b should be monophyletic in (a,(c,b))\n",
				test_name);
		return 1;
	}

	append_element(descendants, node_f);
	result = is_monophyletic(descendants, node_e);
	if (MONOPH_FALSE != result) {
		printf ("%s: a,c,b,f should NOT be monophyletic in (c,b)\n",
				test_name);
		return 1;
	}
	
	printf("%s ok.\n", test_name);
	return 0;
}

int main()
{
	int failures = 0;
	printf("Starting canvas test...\n");
	failures += test_is_monophyletic();
	if (0 == failures) {
		printf("All tests ok.\n");
	} else {
		printf("%d test(s) FAILED.\n", failures);
		return 1;
	}

	return 0;
}
