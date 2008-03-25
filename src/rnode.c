#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>

#include "rnode.h"
#include "list.h"
#include "redge.h"

struct rnode *create_rnode(char *label)
{
	struct rnode *node_p;

	node_p = malloc(sizeof(struct rnode));
	if (NULL == node_p) {
		perror(NULL);
		exit(1);
	}
	if (NULL == label) {
		label = "";
	}
	node_p->label = strdup(label);
	node_p->children = create_llist();	
	node_p->parent_edge = NULL;

	node_p->data = NULL;

	return node_p;
}

int children_count(struct rnode *node)
{
	return node->children->count;
}

int is_leaf(struct rnode *node)
{
	return 0 == node->children->count;
}

int is_root(struct rnode *node)
{
	if (NULL == node->parent_edge)
		return 1;
	if (NULL == node->parent_edge->parent_node)
		return 1;
	return 0;
}

int is_inner_node(struct rnode *node)
{
	return 	(!is_leaf(node) && !is_root(node));
}

void dump_rnode(void *arg)
{
	struct rnode *node = (struct rnode *) arg;

	printf ("rnode at %p: %s\n", node, node->label);
}
