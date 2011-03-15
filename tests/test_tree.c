#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// TODO: rm when debugged
#include <sys/time.h>
#include <sys/resource.h>

#include "rnode.h"
#include "link.h"
#include "list.h"
#include "tree_stubs.h"
#include "tree.h"
#include "nodemap.h"
#include "to_newick.h"
#include "hash.h"


int test_reroot()
{
	const char *test_name = "test_reroot";
	struct rooted_tree tree = tree_3();	/* ((A:1,B:1.0)f:2.0,(C:1,(D:1,E:1)g:2)h:3)i; */
	struct hash *map = create_label2node_map(tree.nodes_in_order);	
	struct rnode *node_g = hash_get(map, "g");
	const char *exp = "((D:1,E:1)g:1,(C:1,(A:1,B:1.0)f:5)h:1);";

	reroot_tree(&tree, node_g);

	const char *obt = to_newick(tree.root);
	
	if (strcmp (exp, obt) != 0) {
		printf ("%s: expected '%s', got '%s'.\n", test_name, 
				exp, obt);
		return 1;
	}

	printf ("%s: ok.\n", test_name);
	return 0;
}

int test_reroot_2()
{
	const char *test_name = "test_reroot_2";

	/* A tree whose root has 3 children:
	 * (A:3,B:3,(C:2,(D:1,E:1)f:1)g:1)h; */

	struct rooted_tree tree = tree_5();	
	struct hash *map = create_label2node_map(tree.nodes_in_order);	
	struct rnode *node_f = hash_get(map, "f");
	const char *exp = "((D:1,E:1)f:0.5,(C:2,(A:3,B:3)h:1)g:0.5);";

	reroot_tree(&tree, node_f);

	const char *obt = to_newick(tree.root);
	
	if (strcmp (exp, obt) != 0) {
		printf ("%s: expected '%s', got '%s'.\n", test_name, 
				exp, obt);
		return 1;
	}

	printf ("%s: ok.\n", test_name);
	return 0;
}

int test_collapse_pure_clades()
{
	const char *test_name = __func__;;
	char *exp = "((A:1,B:1.0)f:2.0,C:3)i;";
	/* ((A:1,B:1.0)f:2.0,(C:1,(C:1,C:1)g:2)h:3)i;  - one clade made of
	 * three 'C's */
	struct rooted_tree tree = tree_4();

	collapse_pure_clades(&tree);

	if (! is_leaf(tree.root->last_child)) {
		printf("%s: node h should be a leaf after being collapsed\n",
				test_name);
		return 1;
	}

	char *obt = to_newick(tree.root);

	if (0 != strcmp(exp, obt)) {
		printf ("%s: expected %s, got %s.\n", test_name,
				exp, obt);
		return 1;
	}

	printf ("%s: ok.\n", test_name);
	return 0;
}

int test_leaf_count()
{
	const char *test_name = "test_leaf_count";
	struct rooted_tree tree = tree_3();	/* ((A:1,B:1.0)f:2.0,(C:1,(D:1,E:1)g:2)h:3)i; */

	if (leaf_count(&tree) != 5) {
		printf ("%s: leaf count should be 5, not %d\n", test_name,
				leaf_count(&tree));
		return 1;
	}

	printf ("%s: ok.\n", test_name);
	return 0;
}

int test_get_leaf_labels()
{
	const char *test_name = "test_get_leaf_labels";
	struct rooted_tree tree = tree_7();	/* ((A:1,:1.0)f:2.0,(C:1,(D:1,E:1):2)h:3)i; */
	struct llist *leaf_labels = get_leaf_labels(&tree);
	struct list_elem *el = leaf_labels->head;
	if (strcmp("A", (char *) el->data) != 0) {
		printf ("%s: expected label 'A', got '%s'.\n", test_name, (char *) el->data);
		return 1;
	}
	el = el->next;
	if (strcmp("C", (char *) el->data) != 0) {
		printf ("%s: expected label 'C', got '%s'.\n", test_name, (char *) el->data);
		return 1;
	}
	el = el->next;
	if (strcmp("D", (char *) el->data) != 0) {
		printf ("%s: expected label 'D', got '%s'.\n", test_name, (char *) el->data);
		return 1;
	}
	el = el->next;
	if (strcmp("E", (char *) el->data) != 0) {
		printf ("%s: expected label 'E', got '%s'.\n", test_name, (char *) el->data);
		return 1;
	}
	el = el->next;
	if (NULL != el) {
		printf ("%s: expected end of list.\n", test_name);
		return 1;
	}

	printf ("%s: ok.\n", test_name);
	return 0;

}

int test_get_type()
{
	const char *test_name = "test_get_tree_type()";
	struct rooted_tree cladogram = tree_2();
	struct rooted_tree phylogram = tree_3();
	struct rooted_tree neither = tree_12();

	if (TREE_TYPE_UNKNOWN != cladogram.type) {
		printf ("%s: tree type should be unknown before get_tree_type() is called.\n",
				test_name);
		return 1;
	}

	if (TREE_TYPE_CLADOGRAM != get_tree_type(&cladogram)) {
		printf ("%s: tree should be a cladogram.\n", test_name);
		return 1;
	}

	if (TREE_TYPE_PHYLOGRAM != get_tree_type(&phylogram)) {
		printf ("%s: tree should be a phylogram.\n", test_name);
		return 1;
	}

	if (TREE_TYPE_NEITHER != get_tree_type(&neither)) {
		printf ("%s: tree should be of neither type.\n", test_name);
		return 1;
	}

	printf ("%s: ok.\n", test_name);
	return 0;
}

int test_is_cladogram()
{
	const char *test_name = "test_is_cladogram";

	struct rooted_tree cladogram = tree_2();
	struct rooted_tree phylogram = tree_3();

	if (! is_cladogram(&cladogram)) {
		printf ("%s: is_cladogram() should return true.\n",
				test_name);
		return 1;
	}
	if (is_cladogram(&phylogram)) {
		printf ("%s: is_cladogram() should return false.\n",
				test_name);
		return 1;
	}

	printf ("%s: ok.\n", test_name);
	return 0;
}

int test_nodes_from_labels()
{
	const char *test_name = "test_nodes_from_labels";
	struct rooted_tree tree = tree_3();
      	struct llist *labels = create_llist();
	append_element(labels, "C");	
	append_element(labels, "f");	
	append_element(labels, "D");	
	append_element(labels, "A");	

	struct llist *nodes = nodes_from_labels(&tree, labels);

	struct list_elem *el = nodes->head;
	if (strcmp(((struct rnode *) el->data)->label, "C") != 0) {
		printf ("%s: expected label 'C', got '%s'\n",
				test_name, ((struct rnode *) el->data)->label);
		return 1;
	}
	el = el->next;
	if (strcmp(((struct rnode *) el->data)->label, "f") != 0) {
		printf ("%s: expected label 'f', got '%s'\n", test_name,
				((struct rnode *) el->data)->label);
		return 1;
	}
	el = el->next;
	if (strcmp(((struct rnode *) el->data)->label, "D") != 0) {
		printf ("%s: expected label 'D', got '%s'\n", test_name,
				((struct rnode *) el->data)->label);
		return 1;
	}
	el = el->next;
	if (strcmp(((struct rnode *) el->data)->label, "A") != 0) {
		printf ("%s: expected label 'A', got '%s'\n", test_name,
				((struct rnode *) el->data)->label);
		return 1;
	}
	el = el->next;
	if (NULL != el) {
		printf ("%s: nodes list not terminated.\n", test_name);
		return 1;
	}

	printf ("%s: ok.\n", test_name);
	return 0;
}

static regex_t * compile_regexp(char *regexp_string)
{
	int errcode;
	regex_t *preg = malloc(sizeof(regex_t));
	int cflags = 0;
	if (NULL == preg) {perror(NULL); exit(EXIT_FAILURE);}
	errcode = regcomp(preg, regexp_string, cflags);
	if (errcode) {
		size_t errbufsize = regerror(errcode, preg, NULL, 0);
		char *errbuf = malloc(errbufsize * sizeof(char));
		if (NULL == errbuf) {perror(NULL); exit(EXIT_FAILURE);}
		regerror(errcode, preg, errbuf, errbufsize);
		fprintf (stderr, "%s\n", errbuf);
		exit(EXIT_FAILURE);
	}

	return preg;
}

int test_nodes_from_regexp()
{
	const char *test_name = "test_nodes_from_regexp";
	/* ((HRV_A1:1,HRV_A2:1.0)HRV_A:2.0,(HRV_C:1,(HRV_B1:1,HRV_B2:1)HRV_B:2):3)HRV; */
	struct rooted_tree tree = tree_8();
	struct llist *nodes;
	char *regexp_string;
	struct list_elem *el;
	regex_t * regexp;

	regexp_string = "HRV_A.";
	regexp = compile_regexp(regexp_string);
	nodes = nodes_from_regexp(&tree, regexp);

	el = nodes->head;
	if (strcmp(((struct rnode *) el->data)->label, "HRV_A1") != 0) {
		printf ("%s: expected label 'HRV_A1', got '%s'\n", test_name,
				((struct rnode *) el->data)->label);
		return 1;
	}
	el = el->next;
	if (strcmp(((struct rnode *) el->data)->label, "HRV_A2") != 0) {
		printf ("%s: expected label 'HRV_A2', got '%s'\n", test_name,
				((struct rnode *) el->data)->label);
		return 1;
	}
	el = el->next;
	if (NULL != el) {
		printf ("%s: nodes list not terminated.\n", test_name);
		return 1;
	}

	regexp_string = "HRV_[AB]$";
	regexp = compile_regexp(regexp_string);
	nodes = nodes_from_regexp(&tree, regexp);

	el = nodes->head;
	if (strcmp(((struct rnode *) el->data)->label, "HRV_A") != 0) {
		printf ("%s: expected label 'HRV_A1', got '%s'\n", test_name,
				((struct rnode *) el->data)->label);
		return 1;
	}
	el = el->next;
	if (strcmp(((struct rnode *) el->data)->label, "HRV_B") != 0) {
		printf ("%s: expected label 'HRV_A2', got '%s'\n", test_name,
				((struct rnode *) el->data)->label);
		return 1;
	}
	el = el->next;
	if (NULL != el) {
		printf ("%s: nodes list not terminated.\n", test_name);
		return 1;
	}

	printf ("%s: ok.\n", test_name);
	return 0;
}

int main()
{
	int failures = 0;
	printf("Starting tree test...\n");
	failures += test_reroot();
	failures += test_reroot_2();
	failures += test_collapse_pure_clades();
	failures += test_leaf_count();
	failures += test_get_leaf_labels();
	failures += test_get_type();
	failures += test_is_cladogram();
	failures += test_nodes_from_labels();
	failures += test_nodes_from_regexp();
	if (0 == failures) {
		printf("All tests ok.\n");
	} else {
		printf("%d test(s) FAILED.\n", failures);
		return 1;
	}

	return 0;
}

