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
	
	node_A = create_rnode("A", "");
	node_B = create_rnode("B", "");
	node_C = create_rnode("C", "");
	node_e = create_rnode("", "");
	node_d = create_rnode("", "");

	add_child(node_d, node_A);
	add_child(node_d, node_B);
	add_child(node_e, node_d);
	add_child(node_e, node_C);

	nodes_in_order = create_llist();
	append_element(nodes_in_order, node_A);
	append_element(nodes_in_order, node_B);
	append_element(nodes_in_order, node_d);
	append_element(nodes_in_order, node_C);
	append_element(nodes_in_order, node_e);

	result.root = node_e;
	result.nodes_in_order = nodes_in_order;
	result.type = TREE_TYPE_UNKNOWN;

	return result;
}

/* ((A,B)f,(C,(D,E)g)h)i; */
struct rooted_tree tree_2()
{
	struct rnode *node_A, *node_B, *node_C, *node_D, *node_E;
	struct rnode *node_f, *node_g, *node_h, *node_i;
	struct llist *nodes_in_order;
	struct rooted_tree result;
	
	node_A = create_rnode("A", "");
	node_B = create_rnode("B", "");
	node_C = create_rnode("C", "");
	node_D = create_rnode("D", "");
	node_E = create_rnode("E", "");
	node_f = create_rnode("f", "");
	node_g = create_rnode("g", "");
	node_h = create_rnode("h", "");
	node_i = create_rnode("i", "");

	add_child(node_f, node_A);
	add_child(node_f, node_B);
	add_child(node_g, node_D);
	add_child(node_g, node_E);
	add_child(node_h, node_C);
	add_child(node_h, node_g);
	add_child(node_i, node_f);
	add_child(node_i, node_h);

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
	result.type = TREE_TYPE_UNKNOWN;

	return result;
}

/* ((A:1,B:1.0)f:2.0,(C:1,(D:1,E:1)g:2)h:3)i; */
struct rooted_tree tree_3()
{
	struct rnode *node_A, *node_B, *node_C, *node_D, *node_E;
	struct rnode *node_f, *node_g, *node_h, *node_i;
	struct llist *nodes_in_order;
	struct rooted_tree result;
	
	node_A = create_rnode("A", "1");
	node_B = create_rnode("B", "1.0");
	node_C = create_rnode("C", "1");
	node_D = create_rnode("D", "1");
	node_E = create_rnode("E", "1");
	node_f = create_rnode("f", "2.0");
	node_g = create_rnode("g", "2");
	node_h = create_rnode("h", "3");
	node_i = create_rnode("i", "");

	add_child(node_f, node_A);
	add_child(node_f, node_B);
	add_child(node_g, node_D);
	add_child(node_g, node_E);
	add_child(node_h, node_C);
	add_child(node_h, node_g);
	add_child(node_i, node_f);
	add_child(node_i, node_h);

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
	result.type = TREE_TYPE_UNKNOWN;

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
	
	node_A = create_rnode("A", "1");
	node_B = create_rnode("B", "1.0");
	node_C = create_rnode("C", "1");
	node_D = create_rnode("C", "1");	/* intentional */
	node_E = create_rnode("C", "1");	/* intentional */
	node_f = create_rnode("f", "2.0");
	node_g = create_rnode("g", "2");
	node_h = create_rnode("h", "3");
	node_i = create_rnode("i", "");

	add_child(node_f, node_A);
	add_child(node_f, node_B);
	add_child(node_g, node_D);
	add_child(node_g, node_E);
	add_child(node_h, node_C);
	add_child(node_h, node_g);
	add_child(node_i, node_f);
	add_child(node_i, node_h);

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
	result.type = TREE_TYPE_UNKNOWN;

	return result;
}

/* A tree whose root has 3 children: (A:3,B:3,(C:2,(D:1,E:1)f:1)g:1)h; */
struct rooted_tree tree_5()
{
	struct rnode *node_A, *node_B, *node_C, *node_D, *node_E;
	struct rnode *node_f, *node_g, *node_h;
	struct llist *nodes_in_order;
	struct rooted_tree result;
	
	node_A = create_rnode("A", "3");
	node_B = create_rnode("B", "3");
	node_C = create_rnode("C", "2");
	node_D = create_rnode("D", "1");
	node_E = create_rnode("E", "1");
	node_f = create_rnode("f", "1");
	node_g = create_rnode("g", "1");
	node_h = create_rnode("h", "");

	add_child(node_f, node_D);
	add_child(node_f, node_E);
	add_child(node_g, node_C);
	add_child(node_g, node_f);
	add_child(node_h, node_A);
	add_child(node_h, node_B);
	add_child(node_h, node_g);

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
	result.type = TREE_TYPE_UNKNOWN;

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
	
	node_A = create_rnode("A", "1");
	node_B = create_rnode("B", "1");
	node_C = create_rnode("C", "1");
	node_D = create_rnode("D", "2");
	node_e = create_rnode("e", "1");
	node_f = create_rnode("f", "");

	add_child(node_e, node_A);
	add_child(node_e, node_B);
	add_child(node_e, node_C);
	add_child(node_f, node_e);
	add_child(node_f, node_D);

	nodes_in_order = create_llist();
	append_element(nodes_in_order, node_A);
	append_element(nodes_in_order, node_B);
	append_element(nodes_in_order, node_C);
	append_element(nodes_in_order, node_e);
	append_element(nodes_in_order, node_D);
	append_element(nodes_in_order, node_f);

	result.root = node_f;
	result.nodes_in_order = nodes_in_order;
	result.type = TREE_TYPE_UNKNOWN;

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
	
	node_A = create_rnode("A", "1");
	node_B = create_rnode("", "1.0");
	node_C = create_rnode("C", "1");
	node_D = create_rnode("D", "1");
	node_E = create_rnode("E", "1");
	node_f = create_rnode("f", "2.0");
	node_g = create_rnode("", "2");
	node_h = create_rnode("h", "3");
	node_i = create_rnode("i", "");

	add_child(node_f, node_A);
	add_child(node_f, node_B);
	add_child(node_g, node_D);
	add_child(node_g, node_E);
	add_child(node_h, node_C);
	add_child(node_h, node_g);
	add_child(node_i, node_f);
	add_child(node_i, node_h);

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
	result.type = TREE_TYPE_UNKNOWN;

	return result;
}

/* ((HRV_A1:1,HRV_A2:1.0)HRV_A:2.0,(HRV_C:1,(HRV_B1:1,HRV_B2:1)HRV_B:2):3)HRV; */
struct rooted_tree tree_8()
{
	struct rnode *hrva1 = create_rnode("HRV_A1", "1");
	struct rnode *hrva2 = create_rnode("HRV_A2", "1.0");
	struct rnode *hrvb1 = create_rnode("HRV_B1", "1");
	struct rnode *hrvb2 = create_rnode("HRV_B2", "1");
	struct rnode *hrva = create_rnode("HRV_A", "2.0");
	struct rnode *hrvb = create_rnode("HRV_B", "2");
	struct rnode *hrvc = create_rnode("HRV_C", "1");
	struct rnode *hrvcb = create_rnode("", "3");
	struct rnode *root = create_rnode("", "");

	add_child (hrva, hrva1);
	add_child (hrva, hrva2);
	add_child (hrvb, hrvb1);
	add_child (hrvb, hrvb2);
	add_child (hrvcb, hrvc);
	add_child (hrvcb, hrvb);
	add_child (root, hrva);
	add_child (root, hrvcb);

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
	result.type = TREE_TYPE_UNKNOWN;

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

	nA = create_rnode("A", "");
	nB1 = create_rnode("B", "");
	nB2 = create_rnode("B", "");
	nC = create_rnode("C", "");
	nD1 = create_rnode("D", "");
	nD2 = create_rnode("D", "");
	nD3 = create_rnode("D", "");
	ne = create_rnode("e", "");	
	nf = create_rnode("f", "");	
	ng = create_rnode("g", "");	
	nh = create_rnode("h", "");	
	ni = create_rnode("i", "");	
	nj = create_rnode("j", "");	

	add_child(ne, nD1);	
	add_child(ne, nD2);	
	add_child(nf, ne);
	add_child(nf, nD3);
	add_child(ng, nC);	
	add_child(ng, nB1);	
	add_child(nh, nB2);
	add_child(nh, nA);
	add_child(ni, ng);
	add_child(ni, nh);
	add_child(nj, nf);
	add_child(nj, ni);

	append_element(nodes_in_order, nD1);
	append_element(nodes_in_order, nD2);
	append_element(nodes_in_order, nD3);
	append_element(nodes_in_order, nC);
	append_element(nodes_in_order, nB1);
	append_element(nodes_in_order, nB2);
	append_element(nodes_in_order, nA);

	tree.root = nj;
	tree.nodes_in_order = nodes_in_order;
	tree.type = TREE_TYPE_UNKNOWN;

	return tree;
}

/* A completely linear tree */
/* (((((Homo_sapiens)Homo)Hominini)Homininae)Hominidae)Hominoidea; */
struct rooted_tree tree_10()
{
	struct rnode *homo_sapiens = create_rnode("Homo_sapiens", "");
	struct rnode *homo = create_rnode("Homo", "");
	struct rnode *hominini = create_rnode("Hominini", "");
	struct rnode *homininae = create_rnode("Homininae", "");
	struct rnode *hominidae = create_rnode("Hominidae", "");
	struct rnode *hominoidea = create_rnode("Hominoidea", "");
	struct llist *nodes_in_order = create_llist();
	struct rooted_tree tree;

	add_child(hominoidea, hominidae);	
	add_child(hominidae, homininae);	
	add_child(homininae, hominini);	
	add_child(hominini, homo);
	add_child(homo, homo_sapiens);

	append_element(nodes_in_order, homo_sapiens);
	append_element(nodes_in_order, homo);
	append_element(nodes_in_order, hominini);
	append_element(nodes_in_order, homininae);
	append_element(nodes_in_order, hominidae);
	append_element(nodes_in_order, hominoidea);

	tree.root = hominoidea;
	tree.nodes_in_order = nodes_in_order;
	tree.type = TREE_TYPE_UNKNOWN;

	return tree;
}

/* A partially linear tree */
/* ((((Gorilla,(Pan,Homo)Hominini)Homininae)Hominidae)Hominoidea); */
struct rooted_tree tree_11()
{
	struct rnode *homo = create_rnode("Homo", "");
	struct rnode *pan = create_rnode("Pan", "");
	struct rnode *gorilla = create_rnode("Gorilla", "");
	struct rnode *hominini = create_rnode("Hominini", "");
	struct rnode *homininae = create_rnode("Homininae", "");
	struct rnode *hominidae = create_rnode("Hominidae", "");
	struct rnode *hominoidea = create_rnode("Hominoidea", "");
	struct llist *nodes_in_order = create_llist();
	struct rooted_tree tree;

	add_child(hominoidea, hominidae);	
	add_child(hominidae, homininae);	
	add_child(homininae, gorilla);
	add_child(homininae, hominini);	
	add_child(hominini, pan);
	add_child(hominini, homo);

	append_element(nodes_in_order, gorilla);
	append_element(nodes_in_order, pan);
	append_element(nodes_in_order, homo);
	append_element(nodes_in_order, hominini);
	append_element(nodes_in_order, homininae);
	append_element(nodes_in_order, hominidae);
	append_element(nodes_in_order, hominoidea);

	tree.root = hominoidea;
	tree.nodes_in_order = nodes_in_order;
	tree.type = TREE_TYPE_UNKNOWN;

	return tree;
}

/* A tree some edges of which have length, and others don't */
/* ((A,B:1),C); */
struct rooted_tree tree_12()
{
	struct rnode *node_A, *node_B, *node_C, *node_d, *node_e;
	struct llist *nodes_in_order;
	struct rooted_tree result;
	
	node_A = create_rnode("A", "");
	node_B = create_rnode("B", "1");
	node_C = create_rnode("C", "");
	node_e = create_rnode("", "");
	node_d = create_rnode("", "");

	add_child(node_d, node_A);
	add_child(node_d, node_B);
	add_child(node_e, node_d);
	add_child(node_e, node_C);

	nodes_in_order = create_llist();
	append_element(nodes_in_order, node_A);
	append_element(nodes_in_order, node_B);
	append_element(nodes_in_order, node_d);
	append_element(nodes_in_order, node_C);
	append_element(nodes_in_order, node_e);

	result.root = node_e;
	result.nodes_in_order = nodes_in_order;
	result.type = TREE_TYPE_UNKNOWN;

	return result;
}

/* A tree with short labels, out of alphabetical order */
/* ((Bee,Ant),(Eel,(Dog,Cat))); */
struct rooted_tree tree_13()
{
	struct rnode *node_A, *node_B, *node_C, *node_D, *node_E;
	struct rnode *node_f, *node_g, *node_h, *node_i;
	struct llist *nodes_in_order;
	struct rooted_tree result;
	
	node_A = create_rnode("Ant", "");
	node_B = create_rnode("Bee", "");
	node_C = create_rnode("Cat", "");
	node_D = create_rnode("Dog", "");
	node_E = create_rnode("Eel", "");
	node_f = create_rnode("", "");
	node_g = create_rnode("", "");
	node_h = create_rnode("", "");
	node_i = create_rnode("", "");

	add_child(node_f, node_B);
	add_child(node_f, node_A);
	add_child(node_g, node_D);
	add_child(node_g, node_C);
	add_child(node_h, node_E);
	add_child(node_h, node_g);
	add_child(node_i, node_f);
	add_child(node_i, node_h);

	nodes_in_order = create_llist();
	append_element(nodes_in_order, node_B);
	append_element(nodes_in_order, node_A);
	append_element(nodes_in_order, node_f);
	append_element(nodes_in_order, node_D);
	append_element(nodes_in_order, node_C);
	append_element(nodes_in_order, node_g);
	append_element(nodes_in_order, node_E);
	append_element(nodes_in_order, node_h);
	append_element(nodes_in_order, node_i);

	result.root = node_i;
	result.nodes_in_order = nodes_in_order;
	result.type = TREE_TYPE_UNKNOWN;

	return result;
}

/* A top-light ladder tree */
/* (Petromyzon,(Carcharodon,(Xenopus,(Columba,(Equus,Homo)Mammalia)Amniota)Tetrapoda)Gnathostomata)Vertebrata;
 * */
struct rooted_tree tree_14()
{
	struct rnode *Petromyzon = create_rnode("Petromyzon", "");
	struct rnode *Carcharodon = create_rnode("Carcharodon", "");
	struct rnode *Xenopus = create_rnode("Xenopus", "");
	struct rnode *Columba = create_rnode("Columba", "");
	struct rnode *Equus = create_rnode("Equus", "");
	struct rnode *Homo = create_rnode("Homo", "");
	struct rnode *Mammalia = create_rnode("Mammalia", "");
	struct rnode *Amniota = create_rnode("Amniota", "");
	struct rnode *Tetrapoda = create_rnode("Tetrapoda", "");
	struct rnode *Gnathostomata = create_rnode("Gnathostomata", "");
	struct rnode *Vertebrata = create_rnode("Vertebrata", "");

	struct llist *nodes_in_order;
	struct rooted_tree result;
	
	add_child(Vertebrata, Petromyzon);
	add_child(Vertebrata, Gnathostomata);
	add_child(Gnathostomata, Carcharodon);
	add_child(Gnathostomata, Tetrapoda);
	add_child(Tetrapoda, Xenopus);
	add_child(Tetrapoda, Amniota);
	add_child(Amniota, Columba);
	add_child(Amniota, Mammalia);
	add_child(Mammalia, Equus);
	add_child(Mammalia, Homo);

	nodes_in_order = create_llist();
	append_element(nodes_in_order, Petromyzon);
	append_element(nodes_in_order, Carcharodon);
	append_element(nodes_in_order, Xenopus);
	append_element(nodes_in_order, Columba);
	append_element(nodes_in_order, Equus);
	append_element(nodes_in_order, Homo);
	append_element(nodes_in_order, Mammalia);
	append_element(nodes_in_order, Amniota);
	append_element(nodes_in_order, Tetrapoda);
	append_element(nodes_in_order, Gnathostomata);
	append_element(nodes_in_order, Vertebrata);

	result.root = Vertebrata;
	result.nodes_in_order = nodes_in_order;
	result.type = TREE_TYPE_CLADOGRAM; 	/* should make no difference */

	return result;
}

/* A top-heavy ladder tree, isomorphic to #14 */
/* (((((Equus,Homo)Mammalia,Columba)Amniota,Xenopus)Tetrapoda,Carcharodon)Gnathostomata,Petromyzon)Vertebrata; */
struct rooted_tree tree_15()
{
	struct rnode *Petromyzon = create_rnode("Petromyzon", "");
	struct rnode *Carcharodon = create_rnode("Carcharodon", "");
	struct rnode *Xenopus = create_rnode("Xenopus", "");
	struct rnode *Columba = create_rnode("Columba", "");
	struct rnode *Equus = create_rnode("Equus", "");
	struct rnode *Homo = create_rnode("Homo", "");
	struct rnode *Mammalia = create_rnode("Mammalia", "");
	struct rnode *Amniota = create_rnode("Amniota", "");
	struct rnode *Tetrapoda = create_rnode("Tetrapoda", "");
	struct rnode *Gnathostomata = create_rnode("Gnathostomata", "");
	struct rnode *Vertebrata = create_rnode("Vertebrata", "");

	struct llist *nodes_in_order;
	struct rooted_tree result;
	
	add_child(Mammalia, Equus);
	add_child(Mammalia, Homo);
	add_child(Amniota, Mammalia);
	add_child(Amniota, Columba);
	add_child(Tetrapoda, Amniota);
	add_child(Tetrapoda, Xenopus);
	add_child(Gnathostomata, Tetrapoda);
	add_child(Gnathostomata, Carcharodon);
	add_child(Vertebrata, Gnathostomata);
	add_child(Vertebrata, Petromyzon);

	nodes_in_order = create_llist();
	append_element(nodes_in_order, Equus);
	append_element(nodes_in_order, Homo);
	append_element(nodes_in_order, Mammalia);
	append_element(nodes_in_order, Columba);
	append_element(nodes_in_order, Amniota);
	append_element(nodes_in_order, Xenopus);
	append_element(nodes_in_order, Tetrapoda);
	append_element(nodes_in_order, Carcharodon);
	append_element(nodes_in_order, Gnathostomata);
	append_element(nodes_in_order, Petromyzon);
	append_element(nodes_in_order, Vertebrata);

	result.root = Vertebrata;
	result.nodes_in_order = nodes_in_order;
	result.type = TREE_TYPE_CLADOGRAM; 	/* should make no difference */

	return result;
}

/* A tree with labels that have spaces, or quotes */
/* (('Falco peregrinus', Falco_eleonorae),'Falco_rusticolus'); */
struct rooted_tree tree_16()
{
	struct rnode *peregrinus = create_rnode("'Falco peregrinus'", "");
	struct rnode *eleonorae = create_rnode("Falco_eleonorae", "");
	struct rnode *pereg_eleo = create_rnode("", "");
	struct rnode *rusticolus = create_rnode("'Falco_rusticolus'", "");
	struct rnode *root = create_rnode("", "");

	struct llist *nodes_in_order;
	struct rooted_tree result;
	
	add_child(pereg_eleo, peregrinus);
	add_child(pereg_eleo, eleonorae);
	add_child(root, pereg_eleo);
	add_child(root, rusticolus);

	nodes_in_order = create_llist();
	append_element(nodes_in_order, peregrinus);
	append_element(nodes_in_order, eleonorae);
	append_element(nodes_in_order, pereg_eleo);
	append_element(nodes_in_order, rusticolus);
	append_element(nodes_in_order, root);

	result.root = root;
	result.nodes_in_order = nodes_in_order;
	result.type = TREE_TYPE_CLADOGRAM; 	/* should make no difference */

	return result;
}

