#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "common.h"
#include "rnode.h"
#include "link.h"
#include "list.h"
#include "tree_stubs.h"
#include "tree.h"
#include "nodemap.h"
#include "to_newick.h"
#include "hash.h"

/* Many node functions are tested here because it's easier to check them if we
 * can use the methods in link.h and tree.h - strictly speaking, we can't rely
 * on them when checking rnode.h */

int test_children_count()
{
	const char *test_name = __func__;

	char *length1 = "12.34";
	char *length2 = "2.345";
	char *length3 = "456.7";
	struct rnode *parent, *kid1, *kid2, *kid3;

	parent = create_rnode("parent", "");

	kid1 = create_rnode("kid1", length1);	
	kid2 = create_rnode("kid2", length2);	
	kid3 = create_rnode("kid3", length3);	

	add_child(parent, kid1);
	add_child(parent, kid2);
	add_child(parent, kid3);

	if (children_count(parent) != 3) {
		printf("%s: children_count should be 3, not %d.\n",
				test_name, children_count(parent));
		return 1;
	}

	printf("%s ok.\n", test_name);
	return 0;
}

int test_is_leaf()
{
	const char *test_name = __func__;

	struct rnode *root;
	struct rnode *leaf;

	root = create_rnode("root", "");
	leaf = create_rnode("leaf", "12.34");

	add_child(root, leaf);

	if (is_leaf(root)) {
		printf("%s: root should not be leaf.\n", test_name);
		return 1;
	}
	if (! is_leaf(leaf)) {
		printf("%s: leaf should be leaf.\n", test_name);
		return 1;
	}

	printf("%s ok.\n", test_name);
	return 0;
}

int test_add_3_children()
{
	const char *test_name = __func__;

	char *length1 = "12.34";
	char *length2 = "2.345";
	char *length3 = "456.7";
	struct rnode *parent, *kid1, *kid2, *kid3, *node;

	parent = create_rnode("parent", "");
	kid1 = create_rnode("kid1", length1);	
	kid2 = create_rnode("kid2", length2);	
	kid3 = create_rnode("kid3", length3);	
	add_child(parent, kid1);
	add_child(parent, kid2);
	add_child(parent, kid3);

	if (NULL == parent->first_child) {
		printf("%s: parent->first_child should not be NULL.", test_name);
		return 1;
	}
	if (NULL == parent->last_child) {
		printf("%s: parent->last_child should not be NULL.", test_name);
		return 1;
	}

	if (parent->child_count != 3) {
		printf("%s: children count should be 3 instead of %d.\n",
				test_name, parent->child_count);
		return 1;
	}
	node = parent->first_child;
	if (strcmp(node->edge_length_as_string, length1) != 0) {
		printf("%s: length should be %s.\n", test_name, length1);
		return 1;
	}
	node = node->next_sibling;
	if (strcmp(node->edge_length_as_string, length2) != 0) {
		printf("%s: length should be %s.\n", test_name, length2);
		return 1;
	}
	node = node->next_sibling;
	if (strcmp(node->edge_length_as_string, length3) != 0) {
		printf("%s: length should be %s.\n", test_name, length3);
		return 1;
	}

	printf("%s ok.\n", test_name);
	return 0;
}

int test_simple_tree()
{
	const char *test_name = __func__;

	struct rnode *root;
	struct rnode *left;
	struct rnode *right;

	struct rnode *node;
	char *label;

	root = create_rnode("root","");
	left = create_rnode("left", "12.34");
	right = create_rnode("right", "23.45");

	add_child(root, left);
	add_child(root, right);

	if (children_count(root) != 2) {
		printf ("%s: expected 2 children, not %d.\n",
				test_name, children_count(root));
		return 1;
	}

	node = root->first_child;
	label = node->label;
	if (strcmp("left", label) != 0) {
		printf ("%s: wrong label '%s' (expected 'left')\n",
				test_name, label);
		return 1;
	}

	node = node->next_sibling;
	label = node->label;
	if (strcmp("right", label) != 0) {
		printf ("%s: wrong label '%s' (expected 'right')\n",
				test_name, label);
		return 1;
	}

	printf("%s ok.\n", test_name);
	return 0;
}

int test_is_root()
{
	const char *test_name = "test_is_root";

	struct rnode *root;
	struct rnode *left;
	struct rnode *right;

	root = create_rnode("root", "12.34");
	left = create_rnode("left", "23.45");
	right = create_rnode("right", "");

	add_child(root, left);
	add_child(root, right);

	if (! is_root(root)) {
		printf ("%s: expected is_root() to be true (no parent).\n", test_name);
		return 1;
	}

	/* if node has a parent edge, but this edge has a NULL parent_node,
	 * is_root() should also return true. */
	if (! is_root(root)) {
		printf ( "%s: expected is_root() to be true (parent edge has NULL parent node).\n",
				test_name);
		return 1;
	}

	printf("%s ok.\n", test_name);
	return 0;
}

int test_singleton()
{
	const char *test_name = __func__;

	struct rnode *singleton = create_rnode("single", "");

	if (! is_root(singleton)) {
		printf ("%s: expected singleton to be root.\n", test_name);
		return 1;
	}
	if (! is_leaf(singleton)) {
		printf ("%s: expected singleton to be leaf.\n", test_name);
		return 1;
	}

	printf("%s ok.\n", test_name);
	return 0;
}

int test_insert_node_above()
{
	const char *test_name = __func__;

	struct rooted_tree tree;
	struct rnode *node_f, *node_h, *root;
	struct hash *map;
	char *exp = "(((A,B)f)k,(C,(D,E)g)h)i;";

	tree = tree_2();	/* ((A,B)f,(C,(D,E)g)h)i; - see tree_stubs.h */
	map = create_label2node_map(tree.nodes_in_order);
	node_f = hash_get(map, "f");
	node_h = hash_get(map, "h");
	root = hash_get(map, "i");

	insert_node_above(node_f, "k");

	struct rnode *node_k = node_f->parent;
	if (strcmp("k", node_k->label) != 0) {
		printf ("%s: expected label 'k' for node k, got '%s'.\n",
				test_name, node_k->label);
		return 1;
	}
	if (NULL == node_k->parent) {
		printf ("%s: node k must have a parent.\n", test_name);
		return 1;
	}
	if (root != node_k->parent) {
		printf ("%s: node k's parent is '%p', should be '%p'.\n",
				test_name, node_k->parent, root);
		return 1;
	}
	if (1 != node_k->child_count) {
		printf ("%s: node k should have 1 child, but has %d\n", test_name, node_k->child_count);
		return 1;
	}
	if (node_k->first_child != node_f) {
		printf ("%s: node k's first child should be f, but is %s\n",
				test_name, node_k->first_child->label);
		return 1;
	}
	if (node_k->last_child != node_f) {
		printf ("%s: node k's last child should be f, but is %s\n",
				test_name, node_k->last_child->label);
		return 1;
	}
	if (root->first_child != node_k) {
		printf ("%s: root's first child is %s, should be k\n",
				test_name, root->first_child->label);
		return 1;
	}
	if (root->last_child != node_h) {
		printf ("%s: root's last child is %s, should be h\n",
				test_name, root->last_child->label);
		return 1;
	}
	if (NULL != node_f->next_sibling) {
		printf ("%s: node f should not have a next sib\n",
				test_name);
		return 1;
	}
	char *obt = to_newick(root);
	if (0 != strcmp(exp, obt)) {
		printf ("%s: expected tree '%s', got '%s'.\n",
				test_name, exp, obt);
		return 1;
	}

	printf("%s ok.\n", test_name);
	return 0;
}

int test_insert_node_above_wlen()
{
	const char *test_name = __func__;

	struct rooted_tree tree;
	struct rnode *node_f, *root;
	struct hash *map;
	char *exp = "(((A:1,B:1.0)f:1)k:1,(C:1,(D:1,E:1)g:2)h:3)i;";


	tree = tree_3();	/* ((A:1,B:1.0)f:2.0,(C:1,(D:1,E:1)g:2)h:3)i; - see tree_stubs.h */
	map = create_label2node_map(tree.nodes_in_order);
	node_f = hash_get(map, "f");
	root = hash_get(map, "i");
	insert_node_above(node_f, "k");

	struct rnode *node_k = node_f->parent;
	if (strcmp("k", node_k->label) != 0) {
		printf ("%s: expected label 'k' for node k, got '%s'.\n",
				test_name, node_k->label);
		return 1;
	}
	if (NULL == node_k->parent) {
		printf ("%s: node k must have a parent.\n", test_name);
		return 1;
	}
	if (root != node_k->parent) {
		printf ("%s: node k's parent is '%p', should be '%p'.\n",
				test_name, node_k->parent, root);
		return 1;
	}
	/* NOTE: Edge length is tested by comparing Newick strings */
	char *obt = to_newick(root);
	if (0 != strcmp(exp, obt)) {
		printf ("%s: expected tree '%s', got '%s'.\n",
				test_name, exp, to_newick(root));
		return 1;
	}

	printf("%s ok.\n", test_name);
	return 0;
}

int test_replace_child()
{
	const char *test_name = __func__;

	struct rnode *parent;
	struct rnode *child_1;
	struct rnode *child_2;
	struct rnode *child_3;
	struct rnode *child_4;

	parent = create_rnode("parent", "");
	child_1 = create_rnode("child_1", "");
	child_2 = create_rnode("child_2", "");
	child_3 = create_rnode("child_3", "");
	child_4 = create_rnode("child_4", "");

	add_child(parent, child_1);
	add_child(parent, child_2);
	add_child(parent, child_3);
	add_child(parent, child_4);
	
	struct rnode *new_child = create_rnode("new", "");

	replace_child (child_3, new_child);

	char *exp = "(child_1,child_2,new,child_4)parent;";
	if (0 != strcmp(exp, to_newick(parent))) {
		printf ("%s: expected '%s', got '%s'\n",
			test_name, exp, to_newick(parent));
		return 1;
	}

	printf("%s ok.\n", test_name);
	return 0;
}

int test_replace_child_wlen()
{
	const char *test_name = __func__;

	struct rnode *parent;
	struct rnode *child_1;
	struct rnode *child_2;
	struct rnode *child_3;
	struct rnode *child_4;

	parent = create_rnode("parent", "");
	child_1 = create_rnode("child_1", "1");
	child_2 = create_rnode("child_2", "2.5");
	child_3 = create_rnode("child_3", "3");
	child_4 = create_rnode("child_4", "4.0");

	add_child(parent, child_1);
	add_child(parent, child_2);
	add_child(parent, child_3);
	add_child(parent, child_4);
	
	struct rnode *new_child = create_rnode("new", "2.3");

	replace_child(child_3, new_child);

	char *exp = "(child_1:1,child_2:2.5,new:2.3,child_4:4.0)parent;";
	if (0 != strcmp(exp, to_newick(parent))) {
		printf ("%s: expected '%s', got '%s'\n",
			test_name, exp, to_newick(parent));
		return 1;
	}

	printf("%s ok.\n", test_name);
	return 0;
}

int test_splice_out()
{
	const char *test_name = __func__;

	struct rooted_tree tree;
	struct rnode *node_h, *root;
	struct hash *map;
	char *exp = "((A,B)f,C,(D,E)g)i;";

	tree = tree_2();	/* ((A,B)f,(C,(D,E)g)h)i; - see tree_stubs.h */
	map = create_label2node_map(tree.nodes_in_order);
	node_h = hash_get(map, "h");
	root = hash_get(map, "i");

	splice_out_rnode(node_h);

	char *obt = to_newick(root);
	if (0 != strcmp(exp, obt)) {
		printf ("%s: expected tree '%s', got '%s'.\n",
				test_name, exp, to_newick(root));
		return 1;
	}

	printf("%s ok.\n", test_name);
	return 0;
}

int test_splice_out_wlen()
{
	const char *test_name = __func__;

	struct rooted_tree tree;
	struct rnode *node_h, *root;
	struct hash *map;
	char *exp = "((A:1,B:1.0)f:2.0,C:4,(D:1,E:1)g:5)i;";

	tree = tree_3();/* ((A:1,B:1.0)f:2.0,(C:1,(D:1,E:1)g:2)h:3)i; - see tree_stubs.h */
	map = create_label2node_map(tree.nodes_in_order);
	node_h = hash_get(map, "h");
	root = hash_get(map, "i");

	splice_out_rnode(node_h);

	char *obt = to_newick(root);
	if (0 != strcmp(exp, obt)) {
		printf ("%s: expected tree '%s', got '%s'.\n",
				test_name, exp, to_newick(root));
		return 1;
	}

	printf("%s ok.\n", test_name);
	return 0;
}

int test_unlink_rnode()
{
	const char *test_name = "test_unlink_rnode()";
	struct hash *map;
	struct rnode *node_A;
	/* ((A:1,B:1.0)f:2.0,(C:1,(D:1,E:1)g:2)h:3)i; */
	struct rooted_tree t = tree_3();

	map = create_label2node_map(t.nodes_in_order);
	node_A = hash_get(map, "A");

	// TODO: test value of unlink_rnode_root_child, use a getter! Do this
	// for all functions that test unlink_rnode()

	enum unlink_rnode_status result = unlink_rnode(node_A);
	switch(result) {
	case UNLINK_RNODE_DONE:
		break;
	case UNLINK_RNODE_ROOT_CHILD:
		printf ("%s: unlink_rnode should return UNLINK_RNODE_DONE, "
			"as node A's parent is not the root\n", test_name);
		return 1;
	case UNLINK_RNODE_ERROR:
		fprintf (stderr, "Memory error -\n");
		exit(EXIT_FAILURE);
	default:
		assert(0); /* programmer error */
	}

	char * exp = "(B:3,(C:1,(D:1,E:1)g:2)h:3)i;";
	char * obt = to_newick(t.root);

	if (strcmp(exp, obt) != 0) {
		printf ("%s: expected %s, got %s\n", test_name, exp, obt);
		return 1;
	}

	printf("%s ok.\n", test_name);
	return 0;
}

int test_unlink_rnode_rad_leaf()
{
	const char *test_name = "test_unlink_rnode_rad_leaf()";
	struct hash *map;
	struct rnode *node_D;
	struct rnode *root_child;
	/*  ((A:1,B:1,C:1)e:1,D:2)f */
	struct rooted_tree t = tree_6();

	map = create_label2node_map(t.nodes_in_order);
	node_D = hash_get(map, "D");

	enum unlink_rnode_status result = unlink_rnode(node_D);
	/* node D was a child of the root, which now has only one child (e) -
	 * this one becomes the new root. */
	switch(result) {
	case UNLINK_RNODE_DONE:
		printf ("%s: unlink_rnode() should have returned UNLINK_RNODE_ROOT_CHILD, but returned UNLINK_RNODE_DONE.\n", test_name);
		return 1;
	case UNLINK_RNODE_ROOT_CHILD:
		root_child = get_unlink_rnode_root_child();
		root_child->parent = NULL;
		t.root = root_child;
		break;
	case UNLINK_RNODE_ERROR:
		fprintf (stderr, "Memory error -\n");
		exit(EXIT_FAILURE);
	default:
		assert(0); /* programmer error */
	}

	char * exp = "(A:1,B:1,C:1)e:1;";
	char * obt = to_newick(t.root);

	if (strcmp(exp, obt) != 0) {
		printf ("%s: expected %s, got %s\n", test_name, exp, obt);
		return 1;
	}

	printf("%s ok.\n", test_name);
	return 0;
}

int test_unlink_rnode_3sibs()
{
	const char *test_name = "test_unlink_rnode_3sibs";

	struct hash *map;
	struct rnode *node_B;
	/*  ((A:1,B:1,C:1)e:1,D:2)f */
	struct rooted_tree t = tree_6();

	map = create_label2node_map(t.nodes_in_order);
	node_B = hash_get(map, "B");

	enum unlink_rnode_status result = unlink_rnode(node_B);
	switch(result) {
	case UNLINK_RNODE_DONE:
		break;
	case UNLINK_RNODE_ROOT_CHILD:
		printf ("%s: unlink_rnode should return UNLINK_RNODE_DONE, "
			"as node A's parent is not the root\n", test_name);
		return 1;
	case UNLINK_RNODE_ERROR:
		fprintf (stderr, "Memory error -\n");
		exit(EXIT_FAILURE);
	default:
		assert(0); /* programmer error */
	}
	
	char * exp = "((A:1,C:1)e:1,D:2)f;";
	char * obt = to_newick(t.root);

	if (strcmp(exp, obt) != 0) {
		printf ("%s: expected %s, got %s\n", test_name, exp, obt);
		return 1;
	}

	printf("%s ok.\n", test_name);
	return 0;

}

int test_remove_children()
{
	const char *test_name = __func__;

	struct rooted_tree tree2 = tree_2();
	struct hash *map = create_label2node_map(tree2.nodes_in_order);
	struct rnode *node_f = hash_get(map, "f");

	remove_children(node_f);

	if (node_f->first_child != 0) {
		printf ("%s: first child should be NULL\n", test_name);
		return 1;
	}
	if (node_f->last_child != 0) {
		printf ("%s: last child should be NULL\n", test_name);
		return 1;
	}
	if (node_f->child_count != 0) {
		printf ("%s: children cound should be 0\n", test_name);
		return 1;
	}

	printf("%s ok.\n", test_name);
	return 0;
}

int test_siblings()
{
	const char *test_name = "test_siblings";

	struct rnode *node_A, *node_B, *node_C;
	struct rnode *node_e, *node_D, *node_f;
	/* ((A,B)f,(C,(D,E)g)h)i; */
	struct rooted_tree tree2 = tree_2();
	/*  ((A:1,B:1,C:1)e:1,D:2)f; */
	struct rooted_tree tree6 = tree_6();
	struct hash *t2map = create_label2node_map(tree2.nodes_in_order);
	struct hash *t6map = create_label2node_map(tree6.nodes_in_order);
	struct llist *sibs;

	/* simple case: tree 2 */
	node_A = hash_get(t2map, "A");
	node_B = hash_get(t2map, "B");
	sibs = siblings(node_A);
	if (sibs->count != 1) {
		printf ("%s: expected exactly 1 sibling, got %d.\n", test_name,
				sibs->count);
		return 1;
	}
	if (sibs->head->data != node_B) {
		printf ("%s: wrong sibling (expected node B, got %s)\n", test_name,
				((struct rnode *) sibs->head->data)->label);
		return 1;
	}

	/* node with >2 children: tree 6 */
	node_A = hash_get(t6map, "A");
	node_B = hash_get(t6map, "B");
	node_C = hash_get(t6map, "C");
	node_D = hash_get(t6map, "D");
	node_e = hash_get(t6map, "e");
	node_f = hash_get(t6map, "f");

	sibs = siblings(node_A);
	if (sibs->count != 2) {
		printf ("%s: expected exactly 2 siblings, got %d.\n", test_name,
				sibs->count);
		return 1;
	}
	if (sibs->head->data != node_B) {
		printf ("%s: wrong sibling (expected node B, got %s)\n", test_name,
				((struct rnode *) sibs->head->data)->label);
		return 1;
	}
	if (sibs->tail->data != node_C) {
		printf ("%s: wrong sibling (expected node C, got %s)\n", test_name,
				((struct rnode *) sibs->head->data)->label);
		return 1;
	}

	sibs = siblings(node_B);
	if (sibs->count != 2) {
		printf ("%s: expected exactly 2 siblings, got %d.\n", test_name,
				sibs->count);
		return 1;
	}
	if (sibs->head->data != node_A) {
		printf ("%s: wrong sibling (expected node A, got %s)\n", test_name,
				((struct rnode *) sibs->head->data)->label);
		return 1;
	}
	if (sibs->tail->data != node_C) {
		printf ("%s: wrong sibling (expected node C, got %s)\n", test_name,
				((struct rnode *) sibs->head->data)->label);
		return 1;
	}

	sibs = siblings(node_C);
	if (sibs->count != 2) {
		printf ("%s: expected exactly 2 siblings, got %d.\n", test_name,
				sibs->count);
		return 1;
	}
	if (sibs->head->data != node_A) {
		printf ("%s: wrong sibling (expected node A, got %s)\n", test_name,
				((struct rnode *) sibs->head->data)->label);
		return 1;
	}
	if (sibs->tail->data != node_B) {
		printf ("%s: wrong sibling (expected node B, got %s)\n", test_name,
				((struct rnode *) sibs->head->data)->label);
		return 1;
	}

	/* Inner node: e */
	sibs = siblings(node_e);
	if (sibs->count != 1) {
		printf ("%s: expected exactly 1 sibling, got %d.\n", test_name,
				sibs->count);
		return 1;
	}
	if (sibs->head->data != node_D) {
		printf ("%s: wrong sibling (expected node D, got %s)\n", test_name,
				((struct rnode *) sibs->head->data)->label);
		return 1;
	}

	/* Leaf with inner sibling: D */
	sibs = siblings(node_D);
	if (sibs->count != 1) {
		printf ("%s: expected exactly 1 sibling, got %d.\n", test_name,
				sibs->count);
		return 1;
	}
	if (sibs->head->data != node_e) {
		printf ("%s: wrong sibling (expected node e, got %s)\n", test_name,
				((struct rnode *) sibs->head->data)->label);
		return 1;
	}

	/* Root should have no sibling */
	sibs = siblings(node_f);
	if (sibs->count != 0) {
		printf ("%s: expected exactly 0 sibling, got %d.\n", test_name,
				sibs->count);
		return 1;
	}

	printf("%s ok.\n", test_name);
	return 0;
}

int test_insert_remove_child_noop()
{
	const char* test_name = __func__;

	struct rnode *mum = create_rnode("mum","");
	struct rnode *kid1 = create_rnode("kid1","");
	struct rnode *kid2 = create_rnode("kid2","");
	struct rnode *kid3 = create_rnode("kid3","");
	struct rnode *kid4 = create_rnode("kid4","");
	struct rnode *grandkid1 = create_rnode("grandkid1", "");
	struct rnode *grandkid2 = create_rnode("grandkid2", "");
	int status;

	add_child(mum, kid1);
	add_child(mum, kid2);
	add_child(mum, kid3);
	add_child(kid4, grandkid1);
	add_child(kid4, grandkid2);

	/* Insertion at negative index should fail, all nodes involved should
	 * be left untouched. */
	/* Ok, we can't check that everything was left alone, but at least we
	 * check the usual suspects. */
	status = insert_child(mum, kid4, -1);
	if (status != FAILURE) {
		printf("%s: insertion at index <0 should fail.\n", test_name);
		return 1;
	}
	if (mum->first_child != kid1) {
		printf ("%s: failed insertion wrongly altered first_child\n",
				test_name);
		return 1;
	}
	if (mum->last_child != kid3) {
		printf ("%s: failed insertion wrongly altered last_child\n",
				test_name);
		return 1;
	}
	if (kid4->parent != NULL) {
		printf ("%s: failed insertion wrongly altered parent\n",
				test_name);
		return 1;
	}
	if (kid1->next_sibling != kid2) {
		printf ("%s: failed insertion wrongly altered next_sibling\n",
				test_name);
		return 1;
	}
	if (kid4->first_child != grandkid1) {
		printf ("%s: failed insertion wrongly altered insert's "
			"first child\n", test_name);
		return 1;
	}
	if (kid4->last_child != grandkid2) {
		printf ("%s: failed insertion wrongly altered insert's "
			"last child\n", test_name);
		return 1;
	}

	/* Insertion at index > #kids should fail */
	status = insert_child(mum, kid4, 4);
	if (status != FAILURE) {
		printf("%s: insertion at index > #children should fail.\n", test_name);
		return 1;
	}
	if (mum->first_child != kid1) {
		printf ("%s: failed insertion wrongly altered first_child\n",
				test_name);
		return 1;
	}
	if (mum->last_child != kid3) {
		printf ("%s: failed insertion wrongly altered last_child\n",
				test_name);
		return 1;
	}
	if (kid4->parent != NULL) {
		printf ("%s: failed insertion wrongly altered parent\n",
				test_name);
		return 1;
	}
	if (kid3->next_sibling != NULL) {
		printf ("%s: failed insertion wrongly altered next_sibling\n",
				test_name);
		return 1;
	}
	if (kid4->first_child != grandkid1) {
		printf ("%s: failed insertion wrongly altered insert's "
			"first child\n", test_name);
		return 1;
	}
	if (kid4->last_child != grandkid2) {
		printf ("%s: failed insertion wrongly altered insert's "
			"last child\n", test_name);
		return 1;
	}

	printf("%s ok.\n", test_name);
	return 0;
}

int test_insert_remove_child_head()
{
	const char * test_name = __func__;

	struct rnode *mum = create_rnode("mum","");
	struct rnode *kid1 = create_rnode("kid1","");
	struct rnode *kid2 = create_rnode("kid2","");
	struct rnode *kid3 = create_rnode("kid3","");
	struct rnode *kid4 = create_rnode("kid4","");
	struct rnode *grandkid1 = create_rnode("grandkid1", "");
	struct rnode *grandkid2 = create_rnode("grandkid2", "");
	struct rnode *node;
	int index = -1;

	add_child(mum, kid1);
	add_child(mum, kid2);
	add_child(mum, kid3);
	add_child(kid4, grandkid1);
	add_child(kid4, grandkid2);

	/* Removal and insertion at the beginning of the list */
	index = remove_child(kid1);
	if (index != 0) {
		printf("%s: expected index 0, got %d\n", test_name, index);
		return 1;
	}
	insert_child(mum, kid4, 0);
	node = mum->first_child;
	if (node != kid4) {
		printf("%s: expected node %p, got %p.\n", test_name, kid4, node);
		return 1;
	}
	if (kid4->next_sibling != kid2) {
		printf("%s: expected kid2 as next sib, got %s.\n", test_name,
				kid4->next_sibling->label);
		return 1;
	}
	if (mum->last_child != kid3) {
		printf ("%s: expected kid3 as last child, got %s\n", test_name,
				mum->last_child->label);
		return 1;
	}
	if (kid4->parent != mum) {
		printf ("%s: expected mum as parent, got %p\n", test_name,
				kid4->parent);
		return 1;
	}
	if (kid4->first_child != grandkid1) {
		printf ("%s: failed insertion wrongly altered insert's "
			"first child\n", test_name);
		return 1;
	}
	if (kid4->last_child != grandkid2) {
		printf ("%s: failed insertion wrongly altered insert's "
			"last child\n", test_name);
		return 1;
	}
				

	printf("%s ok.\n", test_name);
	return 0;
}

int test_insert_remove_child_middle()
{
	const char * test_name = __func__;

	struct rnode *mum = create_rnode("mum","");
	struct rnode *kid1 = create_rnode("kid1","");
	struct rnode *kid2 = create_rnode("kid2","");
	struct rnode *kid3 = create_rnode("kid3","");
	struct rnode *kid4 = create_rnode("kid4","");
	struct rnode *grandkid1 = create_rnode("grandkid1", "");
	struct rnode *grandkid2 = create_rnode("grandkid2", "");
	struct rnode *node;
	int index = -1;

	add_child(mum, kid1);
	add_child(mum, kid2);
	add_child(mum, kid3);
	add_child(kid4, grandkid1);
	add_child(kid4, grandkid2);

	/* Removal and insertion in the middle of the list */
	index = remove_child(kid2);
	if (index != 1) {
		printf("%s: expected index 1, got %d\n", test_name, index);
		return 1;
	}
	if (children_count(mum) != 2) {
		printf("%s: expected 2 children, got %d\n", test_name,
				children_count(mum));
		return 1;
	}
	if (kid2->parent != NULL) {
		printf("%s: kid2 should have no parent (has %p)\n", test_name,
				kid2->parent);
		return 1;
	}
	insert_child(mum, kid4, 1);
	node = mum->first_child->next_sibling;
	if (node != kid4) {
		printf("%s: expected node %p, got %p.\n", test_name, kid4, node);
		return 1;
	}
	if (kid4->parent != mum) {
		printf("%s: kid4 should have %p for parent, got %p\n", test_name,
				mum, kid4->parent);
		return 1;
	}
	if (kid4->next_sibling != kid3) {
		printf ("%s: expected kid3 as next sib, got %s\n", test_name,
				kid4->next_sibling->label);
		return 1;
	}
	if (kid4->first_child != grandkid1) {
		printf ("%s: failed insertion wrongly altered insert's "
			"first child\n", test_name);
		return 1;
	}
	if (kid4->last_child != grandkid2) {
		printf ("%s: failed insertion wrongly altered insert's "
			"last child\n", test_name);
		return 1;
	}


	printf("%s ok.\n", test_name);
	return 0;
}

int test_insert_remove_child_tail()
{
	const char * test_name = __func__;

	struct rnode *mum = create_rnode("mum","");
	struct rnode *kid1 = create_rnode("kid1","");
	struct rnode *kid2 = create_rnode("kid2","");
	struct rnode *kid3 = create_rnode("kid3","");
	struct rnode *kid4 = create_rnode("kid4","");
	struct rnode *grandkid1 = create_rnode("grandkid1", "");
	struct rnode *grandkid2 = create_rnode("grandkid2", "");
	struct rnode *node;
	int index = -1;

	add_child(mum, kid1);
	add_child(mum, kid2);
	add_child(mum, kid3);
	add_child(kid4, grandkid1);
	add_child(kid4, grandkid2);

	/* Removal and insertion at the end of the list */
	index = remove_child(kid3);
	if (index != 2) {
		printf("%s: expected index 2, got %d\n", test_name, index);
		return 1;
	}
	insert_child(mum, kid4, 2);
	node = mum->last_child;
	if (node != kid4) {
		printf("%s: expected node %p, got %p.\n", test_name, kid4, node);
		return 1;
	}
	if (kid2->next_sibling != kid4) {
		printf("%s: expected kid4 as next sib, got %s\n", test_name,
				kid4->next_sibling->label);
		return 1;
	}
	if (kid4->parent != mum) {
		printf ("%s: expected mum for parent, got %p\n", test_name,
				kid4->parent);
		return 1;
	}
	if (kid4->first_child != grandkid1) {
		printf ("%s: failed insertion wrongly altered insert's "
			"first child\n", test_name);
		return 1;
	}
	if (kid4->last_child != grandkid2) {
		printf ("%s: failed insertion wrongly altered insert's "
			"last child\n", test_name);
		return 1;
	}


	printf("%s ok.\n", test_name);
	return 0;
}

int test_swap_nodes()
{
	char * test_name = "test_swap_nodes";

	/* ((A:1,B:1.0)f:2.0,(C:1,(D:1,E:1)g:2)h:3)i; */
	struct rooted_tree tree3 = tree_3(); 
	struct hash *map = create_label2node_map(tree3.nodes_in_order);
	struct rnode *node_h = hash_get(map, "h");
	struct rnode *node_i = hash_get(map, "i"); /* h's parent is i (== root) */
	char * h_length;

	if (node_i != node_h->parent) {
		printf ("%s: h's parent expected to be i, got %s\n", test_name,
				node_h->parent->label);
		return 1;
	}
	h_length = strdup(node_h->edge_length_as_string);	/* will be free()d during swap */

	swap_nodes(node_h);

	/* Now things must be the other way around */

	if (node_h != node_i->parent) {
		printf ("%s: i's parent expected to be h, got %s\n", test_name,
				node_h->parent->label);
		return 1;
	}
	if (node_h->parent != NULL) {
		printf ("%s: h is expected to be root, but has a parent (%s)\n", test_name,
				node_h->parent->label);
		return 1;
	}
	if (strcmp(node_i->edge_length_as_string, h_length) != 0) {
		printf ("%s: i's length should be %s, but is %s\n", test_name,
				h_length, node_i->edge_length_as_string);
		return 1;
	}


	printf("%s ok.\n", test_name);
	return 0;
}

int main()
{
	int failures = 0;
	printf("Starting linking test...\n");
	failures += test_add_3_children();
	failures += test_children_count();
	failures += test_simple_tree();
	failures += test_is_leaf();
	failures += test_is_root();
	failures += test_singleton();
	failures += test_insert_node_above_wlen();
	failures += test_insert_node_above();
	failures += test_splice_out();
	failures += test_splice_out_wlen();
	failures += test_unlink_rnode();
	failures += test_unlink_rnode_rad_leaf();
	failures += test_unlink_rnode_3sibs();
	failures += test_siblings();
	failures += test_remove_children();
	failures += test_insert_remove_child_noop();
	failures += test_insert_remove_child_head();
	failures += test_insert_remove_child_middle();
	failures += test_insert_remove_child_tail();
	failures += test_swap_nodes();
	// failures += test_is_stair();
	if (0 == failures) {
		printf("All tests ok.\n");
	} else {
		printf("%d test(s) FAILED.\n", failures);
		return 1;
	}

	return 0;
}
