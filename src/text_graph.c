/* text_graph.c - functions for drawing trees on a text canvas. */

#include <stdlib.h>
#include <math.h>

#include "canvas.h"
#include "tree.h"
#include "list.h"
#include "simple_node_pos.h"
#include "rnode.h"
#include "redge.h"
#include "node_pos_alloc.h"

static const int LBL_SPACE = 2;
static const int ROOT_SPACE = 1;

/* Writes the nodes to the canvas. Assumes that the edges have been
 * attributed a double value in field 'length' (in this case, it is done in
 * set_node_depth()). */

void write_to_canvas(struct canvas *canvas, struct rooted_tree *tree,
		const double scale, int align_leaves, double dmax)
{
	struct list_elem *elem;

	for (elem = tree->nodes_in_order->head;
			NULL != elem; elem = elem->next) {
		struct rnode *node =  elem->data;
		struct simple_node_pos *pos =  node->data;
		/* For cladograms */
		if (align_leaves && is_leaf(node))
			pos->depth = dmax;

		/* rint() rounds halfay, better than automatic double->int
		 * conversion */
		int h_pos = rint(ROOT_SPACE + (scale * pos->depth));
		int top = rint(2*pos->top);
		int bottom = rint(2*pos->bottom);
		int mid = rint(pos->top+pos->bottom);	/* (2*top + 2*bottom) / 2 */

		/* draw node */
		canvas_draw_vline(canvas, h_pos, top, bottom);
		canvas_write(canvas, h_pos + LBL_SPACE, mid, node->label);
		if (is_root(node)) {
			canvas_write(canvas, 0, mid, "=");
		} else {
			struct rnode *parent = node->parent_edge->parent_node;
			struct simple_node_pos *parent_data = parent->data;
			int parent_h_pos = rint(ROOT_SPACE + (scale * parent_data->depth));
			canvas_draw_hline(canvas, mid, parent_h_pos, h_pos);
		}

	}
}

/* We create a new canvas every time - we could reuse one, but this would then
have to be passed, increasing coupling and diminishing implementation hiding.
What's more, we can't assume that the new tree will fit in the old canvas. */

void display_tree(struct rooted_tree *tree, int width, int align_leaves)
{	
	/* set node positions */
	alloc_simple_node_pos(tree);
	int num_leaves = set_node_vpos_cb(tree,
			set_simple_node_pos_top,
			set_simple_node_pos_bottom,
			get_simple_node_pos_top,
			get_simple_node_pos_bottom);
	struct h_data hd = set_node_depth_cb(tree,
			set_simple_node_pos_depth,
			get_simple_node_pos_depth);
	double scale = -1;
	struct canvas *canvasp;

	/* create canvas and draw nodes on it */
	scale = (width - hd.l_max - ROOT_SPACE - LBL_SPACE) / hd.d_max;
	if (0.0 == hd.d_max ) { scale = 1; } 	/* one-node trees */
	canvasp = create_canvas(width, 2 * num_leaves);
	write_to_canvas(canvasp, tree, scale, align_leaves, hd.d_max);

	/* output */
	canvas_dump(canvasp);

	/* release memory */
	destroy_canvas(canvasp);
}
