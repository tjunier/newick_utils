/* text_graph.c - functions for drawing trees on a text canvas. */

#include <stdlib.h>
#include <string.h>
/*
#include <stdio.h>

*/
#include "rnode.h"
#include "tree.h"
#include "list.h"
#include "redge.h"
#include "node_pos_alloc.h"

int set_node_vpos_cb(struct rooted_tree *t,
		void (*set_node_top)(struct rnode *, double),
		void (*set_node_bottom)(struct rnode *, double),
		double (*get_node_top)(struct rnode *),
		double (*get_node_bottom)(struct rnode *))
{
	int leaf_count = 0;
	struct list_elem *le;
	struct rnode *node;

	for (le = t->nodes_in_order->head; NULL != le; le = le->next) {
		node = le->data;
		if(is_leaf(node)) {
			set_node_top(node, leaf_count);
			set_node_bottom(node, leaf_count);
			leaf_count++;
		} else {
			struct redge *edge;
			struct rnode *top_child, *bottom_child;
			/* top of this node is average of top and bottom of its
			 * top child node */
			edge = (struct redge *) node->children->head->data;
		       	top_child = edge->child_node;
			set_node_top(node, 0.5 * (
					get_node_top(top_child) +
					get_node_bottom(top_child))
					);
			/* same idea for bottom */
			edge = (struct redge *) node->children->tail->data;
		       	bottom_child = edge->child_node;
			set_node_bottom(node, 0.5 * (
					get_node_top(bottom_child) +
					get_node_bottom(bottom_child))
					);
		}
	}

	return leaf_count;
}

struct h_data set_node_depth_cb(struct rooted_tree *tree,
		void (*set_node_depth)(struct rnode *, double),
		double (*get_node_depth)(struct rnode *))
{
	struct llist *nodes_in_reverse_order;
	struct list_elem *elem;
	struct rnode *node;
	int max_label_len = 0;
	double max_leaf_depth = 0.0;
	struct h_data result;

	nodes_in_reverse_order = llist_reverse(tree->nodes_in_order);

	/* set the root's depth to 0 */
	elem = nodes_in_reverse_order->head;
	node = (struct rnode *) elem->data;
	set_node_depth(node, 0.0);

	/* now traverse node list, setting each node's depth to the sum of its
	 * parent edge's length and its parent node's depth. */
	elem = elem->next;
	for (; NULL != elem; elem = elem->next) {
		struct rnode *parent_node;
		node = (struct rnode *) elem->data;
		parent_node = node->parent_edge->parent_node;

		if (0 == strcmp("", node->parent_edge->length_as_string))
			node->parent_edge->length = 1.0;
		else
			node->parent_edge->length = atof(node->parent_edge->length_as_string);

		double node_depth = node->parent_edge->length +
				get_node_depth(parent_node);

		set_node_depth(node, node_depth);
		
		/* Update max depth and max label length */
		if (node_depth > max_leaf_depth) {max_leaf_depth=node_depth;}
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
