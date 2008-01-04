/* text_graphi.c - functions for drawing trees on a text canvas. */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "list.h"
#include "tree.h"
#include "rnode.h"
#include "redge.h"
#include "canvas.h"

#define LBL_SPACE 2
#define ROOT_SPACE 1

/* Used for storing the node's vertical and horizontal position - independent
 * of final medium. Vertical position is measured in leaf order or fractions
 * thereof (for inner nodes), horizontal position ("depth") is measured in edge
 * length. */

struct node_pos {
	double top;
	double bottom;
	double depth;
};

/* used for returning max depth and max label length */

struct h_data {
	int l_max; 	/* lenth of longest label */
	double d_max; 	/* depth of deepest leaf */
};

/* used for passing options and parameters, instead of having globals laying
 * around */

struct parameters {
	int width;
	int debug;
};

/* allocate node_pos structure for each node */

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

/* Sets vertical positions for each node - the data field of each rnode must
 * point to a struct node_pos - see alloc_node_pos(). Returns the number of
 * leaf nodes, which is the maximal vertical position. -- speed junkies: could
 * refactor so that allocate_node_pos() is done in the same loop */

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

	result.l_max = max_label_len;
	result.d_max = max_leaf_depth;
	return result;
}

/* Writes the nodes to the canvas. Assumes that the edges have been
 * attributed a double value in field 'length' (in this case, it is done in
 * set_node_depth()). */

void write_to_canvas(struct canvas *canvas, struct rooted_tree *tree, const double scale)
{
	struct list_elem *elem;

	for (elem = tree->nodes_in_order->head; NULL != elem; elem = elem->next) {
		struct rnode *node = (struct rnode *) elem->data;
		struct node_pos *pos = (struct node_pos *) node->data;
		/* draw node */
		canvas_draw_vline(canvas,
				rint(ROOT_SPACE + (scale * pos->depth)),
				rint(2*pos->top),
				rint(2*pos->bottom)
			);
		canvas_write(canvas,
				rint(ROOT_SPACE + (scale * pos->depth) + LBL_SPACE), 
				rint(pos->top+pos->bottom),
				node->label
				);
		if (is_root(node)) {
			canvas_write(canvas, 0, rint(pos->top+pos->bottom), "=");
		} else {
			canvas_draw_hline(canvas,
				 rint(pos->top + pos->bottom), /* (2*top + 2*bottom) / 2 */
				 rint(ROOT_SPACE + scale * (pos->depth - node->parent_edge->length)),
				 rint(ROOT_SPACE + scale * (pos->depth))
			);
		}

	}
}

/* We create a new canvas every time - we could reuse one, but this would then
have to be passed, increasing coupling and diminishing implementation hiding.
What's more, we can't assume that the new tree will fit in the old canvas. */

void display_tree(struct rooted_tree *tree, int width)
{	
	/* set node positions */
	alloc_node_pos(tree);
	int num_leaves = set_node_vpos(tree);
	struct h_data hd = set_node_depth(tree);
	double scale = -1;
	struct canvas *canvasp;

	if (0.0 == hd.d_max ) { hd.d_max = 1; } 	/* one-node trees */
	/* create canvas and draw nodes on it */
	printf ("width: %d, l_max: %d, ROOT_SPACE: %d, LBL_SPACE: %d, d_max: %g\n",
			width, hd.l_max, ROOT_SPACE, LBL_SPACE, hd.d_max);
	scale = (width - hd.l_max - ROOT_SPACE - LBL_SPACE) / hd.d_max;
	canvasp = create_canvas(width, 2 * num_leaves);
	write_to_canvas(canvasp, tree, scale);

	/* output */
	canvas_dump(canvasp);

	/* release memory */
	destroy_canvas(canvasp);
}
