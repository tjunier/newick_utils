/* text_graph.c - functions for drawing trees on a text canvas. */

#include <math.h>
#include <stdlib.h>
#include <stdio.h>

#include "tree.h"
#include "list.h"
#include "rnode.h"
#include "node_pos.h"
#include "redge.h"

const int ROOT_SPACE = 10;	/* pixels */
const int LBL_SPACE = 10;	/* pixels */

void svg_header()
{
	printf( "<?xml version='1.0' standalone='no'?>"
	   	"<!DOCTYPE svg PUBLIC '-//W3C//DTD SVG 1.1//EN' "
		"'http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd'>");
	printf( "<svg width='100%%' height='100%%' version='1.1' "
		"xmlns='http://www.w3.org/2000/svg'>");
	printf( "<g style='stroke:black;stroke-width:1;"
	    	"font-size:medium;font-weight:normal;font-family:sans'>");
}

/* Prints the nodes to stdout, as SVG, in a <g> element. Assumes that the edges
 * have been attributed a double value in field 'length' (in this case, it is
 * done in set_node_depth()). */

void write_nodes_to_g (struct rooted_tree *tree, const double scale)
{
	struct list_elem *elem;

	for (elem = tree->nodes_in_order->head; NULL != elem; elem = elem->next) {
		struct rnode *node = (struct rnode *) elem->data;
		struct node_pos *pos = (struct node_pos *) node->data;
		/* draw node */
		printf("<line x1='%.4f' y1='%.4f' x2='%.4f' y2='%.4f'/>",
				rint(ROOT_SPACE + (scale * pos->depth)),
				rint(2*pos->top),
				rint(ROOT_SPACE + (scale * pos->depth)),
				rint(2*pos->bottom)
			);
		printf("<text style='stroke:none;font-size:%s' x='%.4f y=%.4f'>%s</text>",
				"medium",
				rint(ROOT_SPACE + (scale * pos->depth) + LBL_SPACE), 
				rint(pos->top+pos->bottom),
				node->label
				);
		if (is_root(node)) {
			printf("<line x1='0' y1='%.4f' x2='%.4f' y2='%.4f'/>",
					rint(pos->top+pos->bottom),
					rint(ROOT_SPACE + (scale * pos->depth)),
					rint(pos->top+pos->bottom));

		} else {
			printf ("<line x1='%.4f' y1='%.4f' x2='%.4f' y2 = '%.4f'/>",
				 rint(ROOT_SPACE + scale * (pos->depth - node->parent_edge->length)),
				 rint(pos->top + pos->bottom), /* (2*top + 2*bottom) / 2 */
				 rint(ROOT_SPACE + scale * (pos->depth)),
				 rint(pos->top + pos->bottom));
		}

	}
}

void display_svg_tree(struct rooted_tree *tree, int width)
{	
	/* set node positions */
	alloc_node_pos(tree);
	int num_leaves = set_node_vpos(tree);
	struct h_data hd = set_node_depth(tree);
	double scale = -1;

	if (0.0 == hd.d_max ) { hd.d_max = 1; } 	/* one-node trees */
	/* create canvas and draw nodes on it */
	scale = (width - hd.l_max - ROOT_SPACE - LBL_SPACE) / hd.d_max;
	write_nodes_to_g(tree, scale);
}

void svg_footer() { printf ("</g></svg>"); }
