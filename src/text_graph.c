/* text_graph.c - functions for drawing trees on a text canvas. */

#include <math.h>
#include <stdlib.h>

#include "tree.h"
#include "canvas.h"
#include "list.h"
#include "rnode.h"
#include "node_pos.h"
#include "redge.h"

#define LBL_SPACE 2
#define ROOT_SPACE 1

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
	scale = (width - hd.l_max - ROOT_SPACE - LBL_SPACE) / hd.d_max;
	canvasp = create_canvas(width, 2 * num_leaves);
	write_to_canvas(canvasp, tree, scale);

	/* output */
	canvas_dump(canvasp);

	/* release memory */
	destroy_canvas(canvasp);
}
