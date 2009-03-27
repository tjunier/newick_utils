#include <stdio.h>
#include <string.h>

#include "tree.h"
#include "list.h"
#include "rnode.h"
#include "redge.h"
#include "hash.h"
#include "node_pos_alloc.h"
#include "svg_graph_common.h"

double leaf_vskip = -1; 	/* Vertical separation of leaves (px) */

void set_svg_leaf_vskip(double skip) { leaf_vskip = skip; }

/* Overall graph height (assuming 1 tree per graph, as always) */

int graph_height(int nb_leaves, int with_scale_bar)
{
	int height = 2 * svg_whole_v_shift + ((nb_leaves -1)  * leaf_vskip);
	if (with_scale_bar) 
		height += scale_bar_height;

	return height;
}

/* Outputs an SVG <g> element with all the tree branches, orthogonal */

void draw_branches_ortho (struct rooted_tree *tree, const double h_scale,
		const double v_scale, int align_leaves, double dmax)
{
	printf("<g "
	       	" style='stroke:black;fill:none;stroke-width:1;"
		"stroke-linecap:round'>"
		);

	struct list_elem *elem;
	for (elem=tree->nodes_in_order->head; NULL!=elem; elem=elem->next) {
		struct rnode *node = elem->data;
		struct svg_data *node_data = (struct svg_data *) node->data;

		/* For cladograms */
		if (align_leaves && is_leaf(node))
			node_data->depth = dmax;

		double svg_h_pos = ROOT_SPACE + (h_scale * node_data->depth);
		double svg_top_pos = v_scale * node_data->top; 
		double svg_bottom_pos = v_scale * node_data->bottom; 
		double svg_mid_pos =
			0.5 * v_scale * (node_data->top+node_data->bottom);

		/* draw node (vertical line), except for leaves */
		if (! is_leaf(node)) {
		printf("<line class='clade_%d' "
			"x1='%.4f' y1='%.4f' x2='%.4f' y2='%.4f'/>",
			node_data->group_nb, svg_h_pos, svg_top_pos, svg_h_pos,
			svg_bottom_pos);
		}
		/* draw horizontal line */
		if (is_root(node)) {
			printf("<line x1='0' y1='%.4f' x2='%.4f' y2='%.4f'/>",
				svg_mid_pos, svg_h_pos, svg_mid_pos);

		} else {
			struct rnode *parent = node->parent_edge->parent_node;
			struct svg_data *parent_data = parent->data;
			double svg_parent_h_pos = ROOT_SPACE + (
				h_scale * parent_data->depth);
			printf ("<line class='clade_%d' "
				"x1='%.4f' y1='%.4f' x2='%.4f' y2='%.4f'/>",
				node_data->group_nb, svg_parent_h_pos,
				 svg_mid_pos, svg_h_pos, svg_mid_pos);
		}
		/* draw ornament, if any */
		if (NULL != node_data->ornament)
			printf ("<g transform='translate(%.4f,%.4f)'>%s</g>",
					svg_h_pos, svg_mid_pos,
					node_data->ornament);

	}
	printf("</g>");
}

/* Prints the node text (labels and lengths) in a <g> element, orthogonal */

void draw_text_ortho (struct rooted_tree *tree, const double h_scale,
		const double v_scale, int align_leaves, double dmax)
{
	printf( "<g style='stroke:none'>");

	struct list_elem *elem;
	for (elem=tree->nodes_in_order->head; NULL!=elem; elem=elem->next) {
		struct rnode *node = elem->data;
		struct svg_data *node_data = (struct svg_data *) node->data;

		/* For cladograms */
		if (align_leaves && is_leaf(node))
			node_data->depth = dmax;

		double svg_h_pos = ROOT_SPACE + (h_scale * node_data->depth);
		double svg_mid_pos =
			0.5 * v_scale * (node_data->top+node_data->bottom);

		char *url = NULL;
		if (url_map) url = hash_get(url_map, node->label);

		char *class;
		if (is_leaf(node))
			class = leaf_label_class;
		else
			class = inner_label_class;

		/* draw label IFF it is nonempty */

		if (0 != strcmp(node->label, ""))
			if (url) printf ("<a %s>", url);
			printf("<text class='%s' "
			       "x='%.4f' y='%.4f'>%s</text>",
				class, svg_h_pos + LBL_SPACE,
				svg_mid_pos + LBL_VOFFSET, node->label);
			if (url) printf ("</a>");

		/* Branch lengths */

		if (! is_root(node)) {
			struct rnode *parent = node->parent_edge->parent_node;
			struct svg_data *parent_data = parent->data;
			double svg_parent_h_pos = ROOT_SPACE + (
				h_scale * parent_data->depth);
			/* Print branch length IFF it is nonempty AND 
			 * requested size is not 0 */
			if (0 != strcmp(node->parent_edge->length_as_string,
						"")) {
				printf("<text class='edge-label' "
					"x='%4f' y='%4f'>%s</text>",
					(svg_h_pos + svg_parent_h_pos) / 2.0,
					edge_length_v_offset + svg_mid_pos,
					node->parent_edge->length_as_string);
			}
		}

	}
	printf("</g>");
}

void display_svg_tree_orthogonal(struct rooted_tree *tree,
		struct h_data hd, int align_leaves, int with_scale_bar,
		char *branch_length_unit)
{
	double h_scale = -1;
	/* This could get more complicated if we print many trees with
	 * different scales. For now, it's fixed. */
	double v_scale = leaf_vskip;

	if (0.0 == hd.d_max) { hd.d_max = 1; } 	/* one-node trees */
	h_scale = (graph_width
			- label_char_width * hd.l_max
			- ROOT_SPACE
			- LBL_SPACE
		) / hd.d_max;

	/* Tree is in a separate group - may be useful when (if?) there are
	 * more than one tree*/
	printf( "<g"
		" transform='translate(0,%d)'"
		">", svg_whole_v_shift);
	/* We draw all the tree's branches in an SVG group of their own, to
	 * facilitate editing via Inkscape, Illustrator, etc. */
	draw_branches_ortho(tree, h_scale, v_scale, align_leaves, hd.d_max);
	/* ... likewise for text */
	draw_text_ortho(tree, h_scale, v_scale, align_leaves, hd.d_max);
	/* Draw scale bar if required */
	if (with_scale_bar) {
		double scalebar_vpos = (double) graph_height(leaf_count(tree),
				with_scale_bar) - svg_whole_v_shift;
		draw_scale_bar(ROOT_SPACE, scalebar_vpos, h_scale, hd.d_max,
			branch_length_unit);
	}
	printf ("</g>");
}
