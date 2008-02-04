#include <stdio.h>
#include <stdlib.h>

#include "list.h"
// #include "newick.tab.h"
#include "tree.h"

struct llist *nodes_in_order;
struct rnode *root;

struct rooted_tree *parse_tree()
{
	struct rooted_tree *tree;

	tree = malloc(sizeof(struct rooted_tree));
	if(NULL == tree) {
		perror(NULL);
		exit(EXIT_FAILURE);
	}

	nodes_in_order = create_llist();
	nwsparse();
	
	if (NULL != root) {
		tree->root = root;
		tree->nodes_in_order = nodes_in_order;
		return tree;
	} else {
		free(tree);
		destroy_llist(nodes_in_order);
		return NULL;
	}
}

