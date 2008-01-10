#include <stdio.h>
#include <check.h>
#include <stdlib.h>

#include "../src/link.h"
#include "../src/rnode.h"
#include "../src/redge.h"
#include "../src/list.h"
#include "../src/tree.h"
#include "../src/nodemap.h"
#include "../src/to_newick.h"

#include "tree_stubs.h"

/* Properties */

/* although this is a property of a node, it's easier to test if we
   can use linking functions, so we test it here rather than in test_rnode. */

START_TEST(test_children_count)
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

	fail_if (children_count(parent_p) != 3,
		"wrong children_count, should be 3");
}
END_TEST

START_TEST(test_is_root)
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

	printf ("root's parent edge: %p\n", root->parent_edge);
	fail_unless (is_root(root), "is_root() is false");

	edge = create_redge("0.123");
	set_parent_edge(root, edge);

	/* if node has a parent edge, but this edge has a NULL parent_node,
	 * is_root() should also return true. */
	fail_if (! is_root(root), "is_root() is false");
}
END_TEST

/* same remark as for test_children_count(). */

START_TEST(test_is_leaf)
{
	const char *test_name = "test_is_leaf";

	struct rnode *root;
	struct rnode *left;

	root = create_rnode("root");
	left = create_rnode("left");

	link_p2c(root, left, "12.34");

	fail_if (is_leaf(root), "root should not be leaf");
	fail_if (! is_leaf(left), "leaf should be leaf");
}
END_TEST

/* Operations */

START_TEST(test_add_child_edge)
{
	char *test_name = "test_add_child_edge";
	char *length_s = "12.34";
	struct rnode *parent_p;
	struct redge *edge_p;
	struct list_elem *elem;

	parent_p = create_rnode("parent");
	fail_if (NULL == parent_p->children, "children ptr should not be NULL.");
	fail_if (0 != parent_p->children->count, "children count should be 0");

	edge_p = create_redge(length_s);
	add_child_edge(parent_p, edge_p);
	fail_if (parent_p->children->count != 1, "children count should be 1");
	elem = parent_p->children->head;
	edge_p = (struct redge *) elem->data;
	fail_if (strcmp(length_s, edge_p->length_as_string) != 0, "wrong length");
}
END_TEST

START_TEST(test_add_3_child_edges)
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

	fail_if (NULL == parent_p->children, "children ptr should not be NULL.");
	fail_if (parent_p->children->count != 3, "children count should be 3");
	elem = parent_p->children->head;
	edge_p = (struct redge *) elem->data;
	fail_if(strcmp(edge_p->length_as_string, length1) != 0, "wrong length");
	elem = elem->next;
	edge_p = (struct redge *) elem->data;
	fail_if(strcmp(edge_p->length_as_string, length2) != 0, "wrong length");
	elem = elem->next;
	edge_p = (struct redge *) elem->data;
	fail_if(strcmp(edge_p->length_as_string, length3) != 0, "wrong length");
}
END_TEST

START_TEST(test_link_p2c)
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

	fail_if (children_count(parent_p) != 1, "parent expected to have 1 child");
	kids_p = parent_p->children;	/* llist */
	elem_p = kids_p->head;
	edge_p = (struct redge *) elem_p->data;
	fail_if (edge_p->parent_node != parent_p, "wrong parent node ptr");
	fail_if (edge_p->child_node != child_p, "wrong child node ptr");
	fail_if (child_p->parent_edge != edge_p, "wrong parent edge ptr");
	fail_if (strcmp(edge_p->length_as_string, "12.34") != 0,
		"wrong edge length");
}
END_TEST

START_TEST(test_link_p2c_null_len)
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

	fail_if (children_count(parent_p) != 1, "parent expected to have 1 child ");
	kids_p = parent_p->children;	/* llist */
	elem_p = kids_p->head;
	edge_p = (struct redge *) elem_p->data;
	fail_if (edge_p->parent_node != parent_p, "wrong parent node ptr");
	fail_if (edge_p->child_node != child_p, "wrong child node ptr");
	fail_if (child_p->parent_edge != edge_p, "wrong parent edge ptr");
}
END_TEST

START_TEST(test_simple_tree)
{
	struct rnode *root;
	struct rnode *left;
	struct rnode *right;

	struct redge *edge;
	char *label;
	char *len_s;

	root = create_rnode("root");
	left = create_rnode("left");
	right = create_rnode("right");

	link_p2c(root, left, "12.34");
	link_p2c(root, right, "23.45");

	fail_if (children_count(root) != 2, "expected 2 children");

	edge = (struct redge *) root->children->head->data;
	label = edge->child_node->label;
	fail_if (strcmp("left", label) != 0, "wrong label");
	len_s = edge->length_as_string;
	fail_if(strcmp(len_s, "12.34") != 0, "wrong length");

	edge = (struct redge *) root->children->head->next->data;
	label = edge->child_node->label;
	fail_if (strcmp("right", label) != 0, "wrong label");
	len_s = edge->length_as_string;
	fail_if(strcmp(len_s, "23.45") != 0, "wrong length");
}
END_TEST

START_TEST(test_set_parent_edge)
{
	struct redge *parent_edge;
	struct rnode *child;

	parent_edge = create_redge(NULL);
	child = create_rnode("glops");

	set_parent_edge(child, parent_edge);	
	
	fail_if (parent_edge->child_node != child, "wrong child");
	fail_if (child->parent_edge != parent_edge, "wrong parent edge");
}
END_TEST

START_TEST(test_insert_node_above)
{
	const char *test_name = "test_insert_node_above";

	struct rooted_tree tree;
	struct rnode *node_f, *root;
	struct node_map *map;
	char *exp = "(((A,B)f)k,(C,(D,E)g)h)i;";

	tree = tree_2();	/* ((A,B)f,(C,(D,E)g)h)i; - see tree_stubs.h */
	map = create_node_map(tree.nodes_in_order);
	node_f = get_node_with_label(map, "f");
	root = get_node_with_label(map, "i");
	insert_node_above(node_f, "k");

	struct rnode *node_k = node_f->parent_edge->parent_node;

	fail_if (strcmp("k", node_k->label) != 0, "wrong label, expected 'k'");
	fail_if (NULL == node_k->parent_edge, "node k must have parent edge");
	fail_if (NULL == node_k->parent_edge->parent_node,
		"node k's parent edge must have parent node");
	fail_if (root != node_k->parent_edge->parent_node,
		"node k's parent should be the root.");
	char *obt = to_newick(root);
	fail_if (0 != strcmp(exp, obt), "wrong Newick");
}
END_TEST

START_TEST(test_insert_node_above_wlen)
{
	const char *test_name = "test_insert_node_above_wlen";

	struct rooted_tree tree;
	struct rnode *node_f, *root;
	struct node_map *map;
	char *exp = "(((A:1,B:1.0)f:1)k:1,(C:1,(D:1,E:1)g:2)h:3)i;";


	tree = tree_3();	/* ((A:1,B:1.0)f:2.0,(C:1,(D:1,E:1)g:2)h:3)i; - see tree_stubs.h */
	map = create_node_map(tree.nodes_in_order);
	node_f = get_node_with_label(map, "f");
	root = get_node_with_label(map, "i");
	insert_node_above(node_f, "k");

	struct rnode *node_k = node_f->parent_edge->parent_node;

	fail_if (strcmp("k", node_k->label) != 0, "wrong label, expected 'k'");
	fail_if (NULL == node_k->parent_edge, "node k must have parent edge");
	fail_if (NULL == node_k->parent_edge->parent_node,
		"node k's parent edge must have parent node");
	fail_if (root != node_k->parent_edge->parent_node,
		"node k's parent should be the root.");
	char *obt = to_newick(root);
	fail_if (0 != strcmp(exp, obt), "wrong Newick");
}
END_TEST

START_TEST(test_replace_child_edge)
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
	fail_if (0 != strcmp(exp, to_newick(parent)), "wrong Newick");
}
END_TEST

START_TEST(test_replace_child_edge_wlen)
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
	fail_if (0 != strcmp(exp, to_newick(parent)), "wrong Newick");
}
END_TEST

START_TEST(test_splice_out)
{
	const char *test_name = "test_splice_out";

	struct rooted_tree tree;
	struct rnode *node_h, *root;
	struct node_map *map;
	char *exp = "((A,B)f,C,(D,E)g)i;";

	tree = tree_2();	/* ((A,B)f,(C,(D,E)g)h)i; - see tree_stubs.h */
	map = create_node_map(tree.nodes_in_order);
	node_h = get_node_with_label(map, "h");
	root = get_node_with_label(map, "i");

	splice_out_rnode(node_h);

	char *obt = to_newick(root);
	fail_if (0 != strcmp(exp, obt), "wrong Newick");
}
END_TEST

START_TEST(test_splice_out_wlen)
{
	const char *test_name = "test_splice_out_wlen";

	struct rooted_tree tree;
	struct rnode *node_h, *root;
	struct node_map *map;
	char *exp = "((A:1,B:1.0)f:2.0,C:4,(D:1,E:1)g:5)i;";

	tree = tree_3();/* ((A:1,B:1.0)f:2.0,(C:1,(D:1,E:1)g:2)h:3)i; - see tree_stubs.h */
	map = create_node_map(tree.nodes_in_order);
	node_h = get_node_with_label(map, "h");
	root = get_node_with_label(map, "i");

	splice_out_rnode(node_h);

	char *obt = to_newick(root);
	fail_if (0 != strcmp(exp, obt), "wrong Newick");
}
END_TEST

START_TEST(test_reverse_edge)
{
	const char *test_name = "test_reverse_edge";
	struct node_map *map;
	struct rnode *node_h;
	struct redge *edge;
	char *exp = "(C:1,(D:1,E:1)g:2,((A:1,B:1.0)f:2.0)i:3)h;";
	struct rooted_tree tree;
	char *obt;

	/* ((A:1,B:1.0)f:2.0,(C:1,(D:1,E:1)g:2)h:3)i; - see tree_stubs.h */
	tree = tree_3();

	map = create_node_map(tree.nodes_in_order);
	node_h = get_node_with_label(map, "h");
	edge = node_h->parent_edge;

	reverse_redge(node_h->parent_edge);

	obt = to_newick(node_h);
	fail_if (0 != strcmp(exp, obt), "wrong Newick");
}
END_TEST

Suite * redge_suite (void)
{
	Suite *s = suite_create ("link");

	TCase *tc_prop = tcase_create ("properties");
	tcase_add_test (tc_prop, test_children_count);
	tcase_add_test (tc_prop, test_is_root);
	tcase_add_test (tc_prop, test_is_leaf);
	suite_add_tcase (s, tc_prop);

	TCase *tc_ops = tcase_create ("operations");
	tcase_add_test (tc_ops, test_add_child_edge);
	tcase_add_test (tc_ops, test_add_3_child_edges);
	tcase_add_test (tc_ops, test_link_p2c);
	tcase_add_test (tc_ops, test_link_p2c_null_len);
	tcase_add_test (tc_ops, test_simple_tree);
	tcase_add_test (tc_ops, test_set_parent_edge);
	tcase_add_test (tc_ops, test_insert_node_above);
	tcase_add_test (tc_ops, test_insert_node_above_wlen);
	tcase_add_test (tc_ops, test_replace_child_edge);
	tcase_add_test (tc_ops, test_replace_child_edge_wlen);
	tcase_add_test (tc_ops, test_splice_out);
	tcase_add_test (tc_ops, test_splice_out_wlen);
	tcase_add_test (tc_ops, test_reverse_edge);
	suite_add_tcase (s, tc_ops);
	return s;
	}

int main (void)
{
   	int number_failed;
       Suite *s = redge_suite ();
       SRunner *sr = srunner_create (s);
       srunner_run_all (sr, CK_ENV);
       number_failed = srunner_ntests_failed (sr);
       srunner_free (sr);
       return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}


