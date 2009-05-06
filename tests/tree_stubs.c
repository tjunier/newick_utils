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

/* ((HRV_A1:1,HRV_A2:1.0)HRV_A:2.0,(HRV_C:1,(HRV_B1:1,HRV_B2:1)HRV_B:2):3)HRV; */
struct rooted_tree tree_8()
{
	struct rnode *hrva1 = create_rnode("HRV_A1");
	struct rnode *hrva2 = create_rnode("HRV_A2");
	struct rnode *hrvb1 = create_rnode("HRV_B1");
	struct rnode *hrvb2 = create_rnode("HRV_B2");
	struct rnode *hrva = create_rnode("HRV_A");
	struct rnode *hrvb = create_rnode("HRV_B");
	struct rnode *hrvc = create_rnode("HRV_C");
	struct rnode *hrvcb = create_rnode("");
	struct rnode *root = create_rnode("");

	link_p2c (hrva, hrva1, "1");
	link_p2c (hrva, hrva2, "1.0");
	link_p2c (hrvb, hrvb1, "1");
	link_p2c (hrvb, hrvb2, "1");
	link_p2c (hrvcb, hrvc, "1");
	link_p2c (hrvcb, hrvb, "2");
	link_p2c (root, hrva, "2.0");
	link_p2c (root, hrvcb, "3");

	struct llist *nodes_in_order = create_llist();
	append_element(nodes_in_order, hrva1);
	append_element(nodes_in_order, hrva2);
	append_element(nodes_in_order, hrva);
	append_element(nodes_in_order, hrvc);
	append_element(nodes_in_order, hrvb1);
	append_element(nodes_in_order, hrvb2);
	append_element(nodes_in_order, hrvb);
	append_element(nodes_in_order, hrvcb);
	append_element(nodes_in_order, root);

	struct rooted_tree result;
	result.root = root;
	result.nodes_in_order = nodes_in_order;

	return result;
}

/* A tree with several redundant labels */
/* (((D,D)e,D)f,((C,B)g,(B,A)h)i)j; */
struct rooted_tree tree_9()
{
	struct rnode *nA, *nB1, *nB2, *nC, *nD1, *nD2, *nD3;
	struct rnode *ne, *nf, *ng, *nh, *ni, *nj;
	struct llist *nodes_in_order = create_llist();
	struct rooted_tree tree;

	nA = create_rnode("A");
	nB1 = create_rnode("B");
	nB2 = create_rnode("B");
	nC = create_rnode("C");
	nD1 = create_rnode("D");
	nD2 = create_rnode("D");
	nD3 = create_rnode("D");
	ne = create_rnode("e");	
	nf = create_rnode("f");	
	ng = create_rnode("g");	
	nh = create_rnode("h");	
	ni = create_rnode("i");	
	nj = create_rnode("j");	

	link_p2c(ne, nD1, "");	
	link_p2c(ne, nD2, "");	
	link_p2c(nf, ne, "");
	link_p2c(nf, nD3, "");
	link_p2c(ng, nC, "");	
	link_p2c(ng, nB1, "");	
	link_p2c(nh, nB2, "");
	link_p2c(nh, nA, "");
	link_p2c(ni, ng, "");
	link_p2c(ni, nh, "");
	link_p2c(nj, nf, "");
	link_p2c(nj, ni, "");

	append_element(nodes_in_order, nD1);
	append_element(nodes_in_order, nD2);
	append_element(nodes_in_order, nD3);
	append_element(nodes_in_order, nC);
	append_element(nodes_in_order, nB1);
	append_element(nodes_in_order, nB2);
	append_element(nodes_in_order, nA);

	tree.root = nj;
	tree.nodes_in_order = nodes_in_order;

	return tree;
}

/* A completely linear tree */
/* (((((Homo_sapiens)Homo)Hominini)Homininae)Hominidae)Hominoidea; */
struct rooted_tree tree_10()
{
	struct rnode *homo_sapiens = create_rnode("Homo_sapiens");
	struct rnode *homo = create_rnode("Homo");
	struct rnode *hominini = create_rnode("Hominini");
	struct rnode *homininae = create_rnode("Homininae");
	struct rnode *hominidae = create_rnode("Hominidae");
	struct rnode *hominoidea = create_rnode("Hominoidea");
	struct llist *nodes_in_order = create_llist();
	struct rooted_tree tree;

	link_p2c(hominoidea, hominidae, "");	
	link_p2c(hominidae, homininae, "");	
	link_p2c(homininae, hominini, "");	
	link_p2c(hominini, homo, "");
	link_p2c(homo, homo_sapiens, "");

	append_element(nodes_in_order, homo_sapiens);
	append_element(nodes_in_order, homo);
	append_element(nodes_in_order, hominini);
	append_element(nodes_in_order, homininae);
	append_element(nodes_in_order, hominidae);
	append_element(nodes_in_order, hominoidea);

	tree.root = hominoidea;
	tree.nodes_in_order = nodes_in_order;

	return tree;
}

/* A partially linear tree */
/* ((((Gorilla,(Pan,Homo)Hominini)Homininae)Hominidae)Hominoidea); */
struct rooted_tree tree_11()
{
	struct rnode *homo = create_rnode("Homo");
	struct rnode *pan = create_rnode("Pan");
	struct rnode *gorilla = create_rnode("Gorilla");
	struct rnode *hominini = create_rnode("Hominini");
	struct rnode *homininae = create_rnode("Homininae");
	struct rnode *hominidae = create_rnode("Hominidae");
	struct rnode *hominoidea = create_rnode("Hominoidea");
	struct llist *nodes_in_order = create_llist();
	struct rooted_tree tree;

	link_p2c(hominoidea, hominidae, "");	
	link_p2c(hominidae, homininae, "");	
	link_p2c(homininae, gorilla, "");
	link_p2c(homininae, hominini, "");	
	link_p2c(hominini, pan, "");
	link_p2c(hominini, homo, "");

	append_element(nodes_in_order, gorilla);
	append_element(nodes_in_order, pan);
	append_element(nodes_in_order, homo);
	append_element(nodes_in_order, hominini);
	append_element(nodes_in_order, homininae);
	append_element(nodes_in_order, hominidae);
	append_element(nodes_in_order, hominoidea);

	tree.root = hominoidea;
	tree.nodes_in_order = nodes_in_order;

	return tree;
}
