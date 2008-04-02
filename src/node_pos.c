/* text_graph.c - functions for drawing trees on a text canvas. */
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "redge.h"
#include "rnode.h"
#include "tree.h"
#include "list.h"
#include "node_pos.h"

void alloc_node_pos(struct rooted_tree *t) 
{
	struct list_elem *le;
	struct rnode *node;

	for (le = t->nodes_in_order->head; NULL != le; le = le->next) {
		node = le->data;
		node->data = malloc(sizeof(struct node_pos));
		if (NULL == node->data) {
			perror(NULL);
			exit(EXIT_FAILURE);
		}
	}
}

/* speed junkies: could * refactor so that allocate_node_pos() is done in the
 * same loop */

int set_node_vpos(struct rooted_tree *t) 
{
	int leaf_count = 0;
	struct list_elem *le;
	struct rnode *node;
	struct node_pos *this_node_pos;

	for (le = t->nodes_in_order->head; NULL != le; le = le->next) {
		node = le->data;
		this_node_pos = (struct node_pos *) node->data;
		if(is_leaf(node)) {
			this_node_pos->top = this_node_pos->bottom = leaf_count;
			leaf_count++;
		} else {
			struct rnode *top_child, *bottom_child;
			struct node_pos *top_child_pos, *bottom_child_pos;
			struct redge *edge;
			/* top of this node is average of top and bottom of its
			 * top child node */
			edge = (struct redge *) node->children->head->data;
		       	top_child = edge->child_node;
			top_child_pos = (struct node_pos*) top_child->data;
			this_node_pos->top = (
				top_child_pos->top +
				top_child_pos->bottom) / 2.0;
			/* same idea for bottom */
			edge = (struct redge *) node->children->tail->data;
		       	bottom_child = edge->child_node;
			bottom_child_pos = (struct node_pos*) bottom_child->data;
			this_node_pos->bottom =  (
				bottom_child_pos->top +
				bottom_child_pos->bottom) / 2.0 ;
		}
	}

	return leaf_count;
}

struct h_data set_node_depth(struct rooted_tree *tree)
{
	struct llist *nodes_in_reverse_order;
	struct list_elem *elem;
	struct rnode *node;
	struct node_pos *pos;
	int max_label_len = 0;
	double max_leaf_depth = 0.0;
	struct h_data result;

	nodes_in_reverse_order = llist_reverse(tree->nodes_in_order);

	/* set the root's depth to 0 */
	elem = nodes_in_reverse_order->head;
	node = (struct rnode *) elem->data;
	pos = (struct node_pos *) node->data;
	pos->depth = 0.0;

	/* now traverse node list, setting each node's depth to the sum of its
	 * parent edge's length and its parent node's depth. */
	elem = elem->next;
	for (; NULL != elem; elem = elem->next) {
		struct rnode *parent_node;
		struct node_pos *parent_pos;
		node = (struct rnode *) elem->data;
		parent_node = node->parent_edge->parent_node;
		pos = (struct node_pos *) node->data;
		parent_pos = (struct node_pos *) parent_node->data;

		if (0 == strcmp("", node->parent_edge->length_as_string))
			node->parent_edge->length = 1.0;
		else
			node->parent_edge->length = atof(node->parent_edge->length_as_string);

		pos->depth = node->parent_edge->length + parent_pos->depth;
		
		if (pos->depth > max_leaf_depth) { max_leaf_depth = pos->depth; }
		if (is_leaf(node) && (NULL != node->label)) {
			int lbl_len = strlen(node->label);
			if (lbl_len > max_label_len) {
				max_label_len = lbl_len;
			}
		}
	}
	destroy_llist(nodes_in_reverse_order);

	result.l_max = max_label_len;
	result.d_max = max_leaf_depth;
	return result;
}
