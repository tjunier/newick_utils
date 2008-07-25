#include <stdio.h>
#include <string.h>

#include "rnode.h"
#include "redge.h"
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

int test_add_child_edge()
{
	char *test_name = "test_add_child_edge";
	char *length_s = "12.34";
	struct rnode *parent_p;
	struct redge *edge_p;
	struct list_elem *elem;

	parent_p = create_rnode("parent");
	if (NULL == parent_p->children) {
		printf("%s: children ptr should not be NULL.", test_name);
		return 1;
	}
	if (0 != parent_p->children->count) {
		printf("%s: children count should be 0, not %d.",
				test_name, parent_p->children->count);
		return 1;
	}
	edge_p = create_redge(length_s);
	add_child_edge(parent_p, edge_p);

	if (parent_p->children->count != 1) {
		printf("%s: children count should be 1, not %d.\n",
				test_name, children_count(parent_p));
		return 1;
	}
	elem = parent_p->children->head;
	edge_p = (struct redge *) elem->data;
	if (strcmp(length_s, edge_p->length_as_string) != 0) {
		printf("%s: length should be %s.\n", test_name, length_s);
		return 1;
	}

	printf("%s ok.\n", test_name);
	return 0;
}

int test_children_count()
{
	const char *test_name = "test_children_count";

	char *length1 = "12.34";
	char *length2 = "2.345";
	char *length3 = "456.7";
	struct rnode *parent_p;
	struct redge *edge_p;

	parent_p = create_rnode("parent");
	edge_p = create_redge(length1);
	add_child_edge(parent_p, edge_p);
	edge_p = create_redge(length2);
	add_child_edge(parent_p, edge_p);
	edge_p = create_redge(length3);
	add_child_edge(parent_p, edge_p);

	if (children_count(parent_p) != 3) {
		printf("%s: children_count should be 3, not %d.\n",
				test_name, children_count(parent_p));
		return 1;
	}
	printf("%s ok.\n", test_name);
	return 0;
}

int test_is_leaf()
{
	const char *test_name = "test_is_leaf";

	struct rnode *root;
	struct rnode *left;

	root = create_rnode("root");
	left = create_rnode("left");

	link_p2c(root, left, "12.34");

	if (is_leaf(root)) {
		printf("%s: root should not be leaf.\n", test_name);
		return 1;
	}
	if (! is_leaf(left)) {
		printf("%s: leaf should be leaf.\n", test_name);
		return 1;
	}

	printf("%s ok.\n", test_name);
	return 0;
}

int test_add_3_child_edges()
{
	const char *test_name = "test_add_3_child_edges";

	char *length1 = "12.34";
	char *length2 = "2.345";
	char *length3 = "456.7";
	struct rnode *parent_p;
	struct redge *edge_p;
	struct list_elem *elem;

	parent_p = create_rnode("parent");
	edge_p = create_redge(length1);
	add_child_edge(parent_p, edge_p);
	edge_p = create_redge(length2);
	add_child_edge(parent_p, edge_p);
	edge_p = create_redge(length3);
	add_child_edge(parent_p, edge_p);

	if (NULL == parent_p->children) {
		printf("%s: children ptr should not be NULL.", test_name);
		return 1;
	}
	if (parent_p->children->count != 3) {
		printf("%s: children count should be 3 instead of %d.\n",
				test_name, children_count(parent_p));
		return 1;
	}
	elem = parent_p->children->head;
	edge_p = (struct redge *) elem->data;
	if (strcmp(edge_p->length_as_string, length1) != 0) {
		printf("%s: length should be %s.\n", test_name, length1);
		return 1;
	}
	elem = elem->next;
	edge_p = (struct redge *) elem->data;
	if (strcmp(edge_p->length_as_string, length2) != 0) {
		printf("%s: length should be %s.\n", test_name, length2);
		return 1;
	}
	elem = elem->next;
	edge_p = (struct redge *) elem->data;
	if (strcmp(edge_p->length_as_string, length3) != 0) {
		printf("%s: length should be %s.\n", test_name, length3);
		return 1;
	}

	printf("%s ok.\n", test_name);
	return 0;
}

int test_link_p2c()
{
	const char *test_name = "test_link_p2c";

	struct rnode *parent_p;
	struct rnode *child_p;
	struct llist *kids_p;
	struct list_elem *elem_p;
	struct redge *edge_p;

	parent_p = create_rnode("parent");
	child_p = create_rnode("child");

	link_p2c(parent_p, child_p, "12.34");

	if (children_count(parent_p) != 1) {
		printf ("%s: parent expected to have 1 child (has %d).\n", 
				test_name, children_count(parent_p));
		return 1;
	}
	kids_p = parent_p->children;	/* llist */
	elem_p = kids_p->head;
	edge_p = (struct redge *) elem_p->data;
	if (edge_p->parent_node != parent_p) {
		printf ("%s: wrong parent node ptr %p (expected %p).\n",
				test_name, edge_p->parent_node,
				parent_p);
		return 1;
	}
	if (edge_p->child_node != child_p) {
		printf ("%s: wrong parent edge ptr %p (expected %p).\n",
				test_name, edge_p->child_node,
				child_p);
		return 1;
	}
	if (child_p->parent_edge != edge_p) {
		printf ("%s: wrong parent edge ptr %p (expected %p).\n",
				test_name, child_p->parent_edge,
				edge_p);
		return 1;
	}
	if (strcmp(edge_p->length_as_string, "12.34") != 0) {
		printf ("%s: wrong edge length %s (should be 12.34)\n",
				test_name, edge_p->length_as_string);
		return 1;
	}
	printf("%s ok.\n", test_name);
	return 0;
}

int test_link_p2c_null_len()
{
	const char *test_name = "test_link_p2c_null_len";

	struct rnode *parent_p;
	struct rnode *child_p;
	struct llist *kids_p;
	struct list_elem *elem_p;
	struct redge *edge_p;

	parent_p = create_rnode("parent");
	child_p = create_rnode("child");

	link_p2c(parent_p, child_p, "1");

	if (children_count(parent_p) != 1) {
		printf ("%s: parent expected to have 1 child (has %d).\n", 
				test_name, children_count(parent_p));
		return 1;
	}
	kids_p = parent_p->children;	/* llist */
	elem_p = kids_p->head;
	edge_p = (struct redge *) elem_p->data;
	if (edge_p->parent_node != parent_p) {
		printf ("%s: wrong parent node ptr %p (expected %p).\n",
				test_name, edge_p->parent_node,
				parent_p);
		return 1;
	}
	if (edge_p->child_node != child_p) {
		printf ("%s: wrong parent edge ptr %p (expected %p).\n",
				test_name, edge_p->child_node,
				child_p);
		return 1;
	}
	if (child_p->parent_edge != edge_p) {
		printf ("%s: wrong parent edge ptr %p (expected %p).\n",
				test_name, child_p->parent_edge,
				edge_p);
		return 1;
	}
	printf("%s ok.\n", test_name);
	return 0;
}

int test_simple_tree()
{
	const char *test_name = "test_simple_tree";

	struct rnode *root;
	struct rnode *left;
	struct rnode *right;

	struct redge *edge;
	char *label;

	root = create_rnode("root");
	left = create_rnode("left");
	right = create_rnode("right");

	link_p2c(root, left, "12.34");
	link_p2c(root, right, "23.45");

	if (children_count(root) != 2) {
		printf ("%s: expected 2 children, not %d.\n",
				test_name, children_count(root));
		return 1;
	}

	edge = (struct redge *) root->children->head->data;
	label = edge->child_node->label;
	if (strcmp("left", label) != 0) {
		printf ("%s: wrong label '%s' (expected 'left')\n",
				test_name, label);
		return 1;
	}

	edge = (struct redge *) root->children->head->next->data;
	label = edge->child_node->label;
	if (strcmp("right", label) != 0) {
		printf ("%s: wrong label '%s' (expected 'right')\n",
				test_name, label);
		return 1;
	}

	printf("%s ok.\n", test_name);
	return 0;
}

int test_set_parent_edge()
{
	const char *test_name = "test_set_parent_edge";

	struct redge *parent_edge;
	struct rnode *child;

	parent_edge = create_redge(NULL);
	child = create_rnode("glops");

	set_parent_edge(child, parent_edge);	
	
	if (parent_edge->child_node != child) {
		printf ("%s: wrong child '%p' (expected '%p')\n",
			test_name, parent_edge->child_node, child);
		return 1;
	}
	if (child->parent_edge != parent_edge) {
		printf ("%s: wrong parent edge '%p' (expected '%p')\n",
			test_name, child->parent_edge, parent_edge);
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

	struct redge *edge;

	root = create_rnode("root");
	left = create_rnode("left");
	right = create_rnode("right");

	link_p2c(root, left, "12.34");
	link_p2c(root, right, "23.45");

	if (! is_root(root)) {
		printf ("%s: expected is_root() to be true (no parent edge).\n", test_name);
		return 1;
	}

	edge = create_redge("0.123");
	set_parent_edge(root, edge);

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

int test_insert_node_above()
{
	const char *test_name = "test_insert_node_above";

	struct rooted_tree tree;
	struct rnode *node_f, *root;
	struct hash *map;
	char *exp = "(((A,B)f)k,(C,(D,E)g)h)i;";

	tree = tree_2();	/* ((A,B)f,(C,(D,E)g)h)i; - see tree_stubs.h */
	map = create_label2node_map(tree.nodes_in_order);
	node_f = hash_get(map, "f");
	root = hash_get(map, "i");
	insert_node_above(node_f, "k");

	struct rnode *node_k = node_f->parent_edge->parent_node;
	if (strcmp("k", node_k->label) != 0) {
		printf ("%s: expected label 'k' for node k, got '%s'.\n",
				test_name, node_k->label);
		return 1;
	}
	if (NULL == node_k->parent_edge) {
		printf ("%s: node k must have parent edge.\n", test_name);
		return 1;
	}
	if (NULL == node_k->parent_edge->parent_node) {
		printf ("%s: node k's parent edge must have parent node.\n", test_name);
		return 1;
	}
	if (root != node_k->parent_edge->parent_node) {
		printf ("%s: node k's parent is '%p', should be '%p'.\n",
				test_name, node_k->parent_edge->parent_node, root);
		return 1;
	}
	char *obt = to_newick(root);
	if (0 != strcmp(exp, obt)) {
		printf ("%s: expected tree '%s', got '%s'.\n",
				test_name, exp, to_newick(root));
		return 1;
	}

	printf("%s ok.\n", test_name);
	return 0;
}

int test_insert_node_above_wlen()
{
	const char *test_name = "test_insert_node_above_wlen";

	struct rooted_tree tree;
	struct rnode *node_f, *root;
	struct hash *map;
	char *exp = "(((A:1,B:1.0)f:1)k:1,(C:1,(D:1,E:1)g:2)h:3)i;";


	tree = tree_3();	/* ((A:1,B:1.0)f:2.0,(C:1,(D:1,E:1)g:2)h:3)i; - see tree_stubs.h */
	map = create_label2node_map(tree.nodes_in_order);
	node_f = hash_get(map, "f");
	root = hash_get(map, "i");
	insert_node_above(node_f, "k");

	struct rnode *node_k = node_f->parent_edge->parent_node;
	if (strcmp("k", node_k->label) != 0) {
		printf ("%s: expected label 'k' for node k, got '%s'.\n",
				test_name, node_k->label);
		return 1;
	}
	if (NULL == node_k->parent_edge) {
		printf ("%s: node k must have parent edge.\n", test_name);
		return 1;
	}
	if (NULL == node_k->parent_edge->parent_node) {
		printf ("%s: node k's parent edge must have parent node.\n", test_name);
		return 1;
	}
	if (root != node_k->parent_edge->parent_node) {
		printf ("%s: node k's parent is '%p', should be '%p'.\n",
				test_name, node_k->parent_edge->parent_node, root);
		return 1;
	}
	char *obt = to_newick(root);
	if (0 != strcmp(exp, obt)) {
		printf ("%s: expected tree '%s', got '%s'.\n",
				test_name, exp, to_newick(root));
		return 1;
	}

	printf("%s ok.\n", test_name);
	return 0;
}

int test_replace_child_edge()
{
	const char *test_name = "test_replace_child_edge";

	struct rnode *parent;
	struct rnode *child_1;
	struct rnode *child_2;
	struct rnode *child_3;
	struct rnode *child_4;
	struct redge *child_1_edge;
	struct redge *child_2_edge;
	struct redge *child_3_edge;
	struct redge *child_4_edge;

	parent = create_rnode("parent");
	child_1 = create_rnode("child_1");
	child_2 = create_rnode("child_2");
	child_3 = create_rnode("child_3");
	child_4 = create_rnode("child_4");
	child_1_edge = create_redge("");
	child_2_edge = create_redge("");
	child_3_edge = create_redge("");
	child_4_edge = create_redge("");

	set_parent_edge(child_1, child_1_edge);
	set_parent_edge(child_2, child_2_edge);
	set_parent_edge(child_3, child_3_edge);
	set_parent_edge(child_4, child_4_edge);

	add_child_edge(parent, child_1_edge);
	add_child_edge(parent, child_2_edge);
	add_child_edge(parent, child_3_edge);
	add_child_edge(parent, child_4_edge);
	
	struct rnode *new_child = create_rnode("new");
	struct redge *new_edge = create_redge("");
	set_parent_edge(new_child, new_edge);

	replace_child_edge(parent, child_3_edge, new_edge);

	char *exp = "(child_1,child_2,new,child_4)parent;";
	if (0 != strcmp(exp, to_newick(parent))) {
		printf ("%s: expected '%s', got '%s'\n",
			test_name, exp, to_newick(parent));
		return 1;
	}

	printf("%s ok.\n", test_name);
	return 0;
}

int test_replace_child_edge_wlen()
{
	const char *test_name = "test_replace_child_edge_wlen";

	struct rnode *parent;
	struct rnode *child_1;
	struct rnode *child_2;
	struct rnode *child_3;
	struct rnode *child_4;
	struct redge *child_1_edge;
	struct redge *child_2_edge;
	struct redge *child_3_edge;
	struct redge *child_4_edge;

	parent = create_rnode("parent");
	child_1 = create_rnode("child_1");
	child_2 = create_rnode("child_2");
	child_3 = create_rnode("child_3");
	child_4 = create_rnode("child_4");
	child_1_edge = create_redge("1");
	child_2_edge = create_redge("2.5");
	child_3_edge = create_redge("3");
	child_4_edge = create_redge("4.0");

	set_parent_edge(child_1, child_1_edge);
	set_parent_edge(child_2, child_2_edge);
	set_parent_edge(child_3, child_3_edge);
	set_parent_edge(child_4, child_4_edge);

	add_child_edge(parent, child_1_edge);
	add_child_edge(parent, child_2_edge);
	add_child_edge(parent, child_3_edge);
	add_child_edge(parent, child_4_edge);
	
	struct rnode *new_child = create_rnode("new");
	struct redge *new_edge = create_redge("2.3");
	set_parent_edge(new_child, new_edge);

	replace_child_edge(parent, child_3_edge, new_edge);

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
	const char *test_name = "test_splice_out";

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
	const char *test_name = "test_splice_out_wlen";

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

int test_reverse_edge()
{
	const char *test_name = "test_reverse_edge";
	struct hash *map;
	struct rnode *node_h;
	struct redge *edge;
	char *exp = "(C:1,(D:1,E:1)g:2,((A:1,B:1.0)f:2.0)i:3)h;";
	struct rooted_tree tree;
	char *obt;

	/* ((A:1,B:1.0)f:2.0,(C:1,(D:1,E:1)g:2)h:3)i; - see tree_stubs.h */
	tree = tree_3();

	map = create_label2node_map(tree.nodes_in_order);
	node_h = hash_get(map, "h");
	edge = node_h->parent_edge;

	reverse_redge(node_h->parent_edge);

	obt = to_newick(node_h);
	if (strcmp(exp, obt) != 0) {
		printf ("%s: expected '%s', got '%s'\n", test_name, exp, obt);
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

	struct rnode *r = unlink_rnode(node_A);
	if (NULL != r) {
		r->parent_edge = NULL;
		t.root = r;
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
	/*  ((A:1,B:1,C:1)e:1,D:2)f */
	struct rooted_tree t = tree_6();

	map = create_label2node_map(t.nodes_in_order);
	node_D = hash_get(map, "D");

	struct rnode *r = unlink_rnode(node_D);
	if (NULL != r) {
		r->parent_edge = NULL;
		t.root = r;
	}
	
	char * exp = "(A:1,B:1,C:1)e;";
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

	struct rnode *r = unlink_rnode(node_B);
	if (NULL != r) {
		r->parent_edge = NULL;
		t.root = r;
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

/* I had second thoughts about condensing stair nodes, as this is a bit a form
 * of cheating. */
/*
int test_is_stair()
{
	return 1;
}
*/

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

int main()
{
	int failures = 0;
	printf("Starting linking test...\n");
	failures += test_add_child_edge();
	failures += test_add_3_child_edges();
	failures += test_children_count();
	failures += test_link_p2c();
	failures += test_link_p2c_null_len();
	failures += test_simple_tree();
	failures += test_is_leaf();
	failures += test_set_parent_edge();
	failures += test_is_root();
	failures += test_insert_node_above_wlen();
	failures += test_insert_node_above();
	failures += test_replace_child_edge();
	failures += test_replace_child_edge_wlen();
	failures += test_splice_out();
	failures += test_splice_out_wlen();
	failures += test_reverse_edge();
	failures += test_unlink_rnode();
	failures += test_unlink_rnode_rad_leaf();
	failures += test_unlink_rnode_3sibs();
	failures += test_siblings();
	// failures += test_is_stair();
	if (0 == failures) {
		printf("All tests ok.\n");
	} else {
		printf("%d test(s) FAILED.\n", failures);
		return 1;
	}

	return 0;
}
