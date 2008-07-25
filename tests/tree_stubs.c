#include <stdlib.h>

#include "rnode.h"
#include "link.h"
#include "list.h"
#include "tree.h"

/* ((A,B),C); */
struct rooted_tree tree_1()
{
	struct rnode *node_A, *node_B, *node_C, *node_d, *node_e;
	struct llist *nodes_in_order;
	struct rooted_tree result;
	
	node_A = create_rnode("A");
	node_B = create_rnode("B");
	node_C = create_rnode("C");
	node_e = create_rnode("");
	node_d = create_rnode("");

	link_p2c(node_d, node_A, NULL);
	link_p2c(node_d, node_B, NULL);
	link_p2c(node_e, node_d, NULL);
	link_p2c(node_e, node_C, NULL);

	nodes_in_order = create_llist();
	append_element(nodes_in_order, node_A);
	append_element(nodes_in_order, node_B);
	append_element(nodes_in_order, node_d);
	append_element(nodes_in_order, node_C);
	append_element(nodes_in_order, node_e);

	result.root = node_e;
	result.nodes_in_order = nodes_in_order;

	return result;
}

/* ((A,B)f,(C,(D,E)g)h)i; */
struct rooted_tree tree_2()
{
	struct rnode *node_A, *node_B, *node_C, *node_D, *node_E;
	struct rnode *node_f, *node_g, *node_h, *node_i;
	struct llist *nodes_in_order;
	struct rooted_tree result;
	
	node_A = create_rnode("A");
	node_B = create_rnode("B");
	node_C = create_rnode("C");
	node_D = create_rnode("D");
	node_E = create_rnode("E");
	node_f = create_rnode("f");
	node_g = create_rnode("g");
	node_h = create_rnode("h");
	node_i = create_rnode("i");

	link_p2c(node_f, node_A, NULL);
	link_p2c(node_f, node_B, NULL);
	link_p2c(node_g, node_D, NULL);
	link_p2c(node_g, node_E, NULL);
	link_p2c(node_h, node_C, NULL);
	link_p2c(node_h, node_g, NULL);
	link_p2c(node_i, node_f, NULL);
	link_p2c(node_i, node_h, NULL);

	nodes_in_order = create_llist();
	append_element(nodes_in_order, node_A);
	append_element(nodes_in_order, node_B);
	append_element(nodes_in_order, node_f);
	append_element(nodes_in_order, node_C);
	append_element(nodes_in_order, node_D);
	append_element(nodes_in_order, node_E);
	append_element(nodes_in_order, node_g);
	append_element(nodes_in_order, node_h);
	append_element(nodes_in_order, node_i);

	result.root = node_i;
	result.nodes_in_order = nodes_in_order;

	return result;
}

/* ((A:1,B:1.0)f:2.0,(C:1,(D:1,E:1)g:2)h:3)i; */
struct rooted_tree tree_3()
{
	struct rnode *node_A, *node_B, *node_C, *node_D, *node_E;
	struct rnode *node_f, *node_g, *node_h, *node_i;
	struct llist *nodes_in_order;
	struct rooted_tree result;
	
	node_A = create_rnode("A");
	node_B = create_rnode("B");
	node_C = create_rnode("C");
	node_D = create_rnode("D");
	node_E = create_rnode("E");
	node_f = create_rnode("f");
	node_g = create_rnode("g");
	node_h = create_rnode("h");
	node_i = create_rnode("i");

	link_p2c(node_f, node_A, "1");
	link_p2c(node_f, node_B, "1.0");
	link_p2c(node_g, node_D, "1");
	link_p2c(node_g, node_E, "1");
	link_p2c(node_h, node_C, "1");
	link_p2c(node_h, node_g, "2");
	link_p2c(node_i, node_f, "2.0");
	link_p2c(node_i, node_h, "3");

	nodes_in_order = create_llist();
	append_element(nodes_in_order, node_A);
	append_element(nodes_in_order, node_B);
	append_element(nodes_in_order, node_f);
	append_element(nodes_in_order, node_C);
	append_element(nodes_in_order, node_D);
	append_element(nodes_in_order, node_E);
	append_element(nodes_in_order, node_g);
	append_element(nodes_in_order, node_h);
	append_element(nodes_in_order, node_i);

	result.root = node_i;
	result.nodes_in_order = nodes_in_order;

	return result;
}

/* A tree with a redundant label */

/* ((A:1,B:1.0)f:2.0,(C:1,(C:1,C:1)g:2)h:3)i; */
struct rooted_tree tree_4()
{
	struct rnode *node_A, *node_B, *node_C, *node_D, *node_E;
	struct rnode *node_f, *node_g, *node_h, *node_i;
	struct llist *nodes_in_order;
	struct rooted_tree result;
	
	node_A = create_rnode("A");
	node_B = create_rnode("B");
	node_C = create_rnode("C");
	node_D = create_rnode("C");	/* intentional */
	node_E = create_rnode("C");	/* intentional */
	node_f = create_rnode("f");
	node_g = create_rnode("g");
	node_h = create_rnode("h");
	node_i = create_rnode("i");

	link_p2c(node_f, node_A, "1");
	link_p2c(node_f, node_B, "1.0");
	link_p2c(node_g, node_D, "1");
	link_p2c(node_g, node_E, "1");
	link_p2c(node_h, node_C, "1");
	link_p2c(node_h, node_g, "2");
	link_p2c(node_i, node_f, "2.0");
	link_p2c(node_i, node_h, "3");

	nodes_in_order = create_llist();
	append_element(nodes_in_order, node_A);
	append_element(nodes_in_order, node_B);
	append_element(nodes_in_order, node_f);
	append_element(nodes_in_order, node_C);
	append_element(nodes_in_order, node_D);
	append_element(nodes_in_order, node_E);
	append_element(nodes_in_order, node_g);
	append_element(nodes_in_order, node_h);
	append_element(nodes_in_order, node_i);

	result.root = node_i;
	result.nodes_in_order = nodes_in_order;

	return result;
}

/* A tree whose root has 3 children: (A:3,B:3,(C:2,(D:1,E:1)f:1)g:1)h; */
struct rooted_tree tree_5()
{
	struct rnode *node_A, *node_B, *node_C, *node_D, *node_E;
	struct rnode *node_f, *node_g, *node_h;
	struct llist *nodes_in_order;
	struct rooted_tree result;
	
	node_A = create_rnode("A");
	node_B = create_rnode("B");
	node_C = create_rnode("C");
	node_D = create_rnode("D");
	node_E = create_rnode("E");
	node_f = create_rnode("f");
	node_g = create_rnode("g");
	node_h = create_rnode("h");

	link_p2c(node_f, node_D, "1");
	link_p2c(node_f, node_E, "1");
	link_p2c(node_g, node_C, "2");
	link_p2c(node_g, node_f, "1");
	link_p2c(node_h, node_A, "3");
	link_p2c(node_h, node_B, "3");
	link_p2c(node_h, node_g, "1");

	nodes_in_order = create_llist();
	append_element(nodes_in_order, node_A);
	append_element(nodes_in_order, node_B);
	append_element(nodes_in_order, node_C);
	append_element(nodes_in_order, node_D);
	append_element(nodes_in_order, node_E);
	append_element(nodes_in_order, node_f);
	append_element(nodes_in_order, node_g);
	append_element(nodes_in_order, node_h);

	result.root = node_h;
	result.nodes_in_order = nodes_in_order;

	return result;
}

/* A tree with an inner node that has three children:
 *  ((A:1,B:1,C:1)e:1,D:2)f; */
struct rooted_tree tree_6()
{
	struct rnode *node_A, *node_B, *node_C, *node_D;
	struct rnode *node_e, *node_f;
	struct llist *nodes_in_order;
	struct rooted_tree result;
	
	node_A = create_rnode("A");
	node_B = create_rnode("B");
	node_C = create_rnode("C");
	node_D = create_rnode("D");
	node_e = create_rnode("e");
	node_f = create_rnode("f");

	link_p2c(node_e, node_A, "1");
	link_p2c(node_e, node_B, "1");
	link_p2c(node_e, node_C, "1");
	link_p2c(node_f, node_e, "1");
	link_p2c(node_f, node_D, "2");

	nodes_in_order = create_llist();
	append_element(nodes_in_order, node_A);
	append_element(nodes_in_order, node_B);
	append_element(nodes_in_order, node_C);
	append_element(nodes_in_order, node_e);
	append_element(nodes_in_order, node_D);
	append_element(nodes_in_order, node_f);

	result.root = node_f;
	result.nodes_in_order = nodes_in_order;

	return result;
}

/* A tree with some empty labels */
/* ((A:1,:1.0)f:2.0,(C:1,(D:1,E:1):2)h:3)i; */
struct rooted_tree tree_7()
{
	struct rnode *node_A, *node_B, *node_C, *node_D, *node_E;
	struct rnode *node_f, *node_g, *node_h, *node_i;
	struct llist *nodes_in_order;
	struct rooted_tree result;
	
	node_A = create_rnode("A");
	node_B = create_rnode("");
	node_C = create_rnode("C");
	node_D = create_rnode("D");
	node_E = create_rnode("E");
	node_f = create_rnode("f");
	node_g = create_rnode("");
	node_h = create_rnode("h");
	node_i = create_rnode("i");

	link_p2c(node_f, node_A, "1");
	link_p2c(node_f, node_B, "1.0");
	link_p2c(node_g, node_D, "1");
	link_p2c(node_g, node_E, "1");
	link_p2c(node_h, node_C, "1");
	link_p2c(node_h, node_g, "2");
	link_p2c(node_i, node_f, "2.0");
	link_p2c(node_i, node_h, "3");

	nodes_in_order = create_llist();
	append_element(nodes_in_order, node_A);
	append_element(nodes_in_order, node_B);
	append_element(nodes_in_order, node_f);
	append_element(nodes_in_order, node_C);
	append_element(nodes_in_order, node_D);
	append_element(nodes_in_order, node_E);
	append_element(nodes_in_order, node_g);
	append_element(nodes_in_order, node_h);
	append_element(nodes_in_order, node_i);

	result.root = node_i;
	result.nodes_in_order = nodes_in_order;

	return result;
}
