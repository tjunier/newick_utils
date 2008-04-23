/* simple_node_pos.c - a structure for storing an rnode's position in a tree,
 * as well as getters and setters. */

#include <stdlib.h>
#include <stdio.h>
/*
#include <string.h>

#include "redge.h"
*/
#include "tree.h"
#include "simple_node_pos.h"
#include "list.h"
#include "rnode.h"

void alloc_simple_node_pos(struct rooted_tree *t) 
{
	struct list_elem *le;
	struct rnode *node;

	for (le = t->nodes_in_order->head; NULL != le; le = le->next) {
		node = le->data;
		node->data = malloc(sizeof(struct simple_node_pos));
		if (NULL == node->data) {
			perror(NULL);
			exit(EXIT_FAILURE);
		}
	}
}

void set_simple_node_pos_top (struct rnode * node, double top)
{
	((struct simple_node_pos *) node->data)->top = top;
}

void set_simple_node_pos_bottom (struct rnode * node, double bottom)
{
	((struct simple_node_pos *) node->data)->bottom = bottom;
}

void set_simple_node_pos_depth (struct rnode * node, double depth)
{
	((struct simple_node_pos *) node->data)->depth = depth;
}

double get_simple_node_pos_top (struct rnode * node)
{
	return ((struct simple_node_pos *) node->data)->top;
}

double get_simple_node_pos_bottom (struct rnode * node)
{
	return ((struct simple_node_pos *) node->data)->bottom;
}

double get_simple_node_pos_depth (struct rnode * node)
{
	return ((struct simple_node_pos *) node->data)->depth;
}

