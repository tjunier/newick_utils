#include <stdio.h>
#include <string.h>

#include "tree.h"
#include "list.h"
#include "rnode.h"
#include "redge.h"
#include "hash.h"
#include "node_pos_alloc.h"
#include "svg_graph_common.h"

static double leaf_vskip = -1; 	/* Vertical separation of leaves (px) */

void set_svg_leaf_vskip(double skip) { leaf_vskip = skip; }

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
			node_data->clade_nb, svg_h_pos, svg_top_pos, svg_h_pos,
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
				node_data->clade_nb, svg_parent_h_pos,
				 svg_mid_pos, svg_h_pos, svg_mid_pos);
		}

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
				svg_mid_pos, node->label);
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

/* Draws a scale bar below the tree. Uses a heuristic to manage horizontal
 * space */

void draw_scale_bar(struct rooted_tree *tree, double h_scale,
		double v_scale, double d_max, char *branch_length_unit)
{
	/* Finds the largest power of 10 that is smaller than the tree's depth.
	 * Then draws as many multiples of this length as possible. If no more
	 * than one can be drawn (because it's longer than half the tree's
	 * depth), then we draw tick marks inside it instead. */

	const int big_tick_height = 5; 			/* px */
	const int small_tick_height = 3;		/* px */
	const int units_text_voffset = -13;		/* px */
	const int vsep = 1;				/* px */
	double vpos = leaf_count(tree) * v_scale; 	/* px */
	double pot = largest_PoT_lte(d_max);		/* tree units */
	double scale_length = pot * h_scale;		/* px */

	printf ("<g transform='translate(%d,%g)'>", ROOT_SPACE, vpos); 

	if (2 * pot > d_max) {
		/* print 1/10th tick marks */
		printf ("<path style='stroke:black' d='M 0 %d l 0 %d "
		       "l %g 0 l 0 %d'/>", vsep, big_tick_height, scale_length,
		       -big_tick_height);
		int i;
		for (i = 1; i < 10; i++)
			printf ("<path d='M %g %d l 0 %d'/>",
				i * (scale_length/10), 
				vsep + big_tick_height - small_tick_height,
				small_tick_height);
		printf ("<text style='font-size:small;stroke:none;"
			"text-anchor:start' x='0' y='0'>0</text>");
		printf ("<text style='font-size:small;stroke:none;"
			"text-anchor:end' x='%g' y='0'>%g</text>",
			scale_length, pot);
		printf ("<text style='font-size:small;stroke:none;"
			"text-anchor:end' x='%g' y='0'>%g</text>",
			scale_length/2, pot/2);
	} else {
		/* print as many multiples of 'scale_length' as  will fit */
		printf ("<path d='M 0 %d l 0 %d'/>", vsep, big_tick_height);
		printf ("<text style='font-size:small;text-anchor:stat;"
			"stroke:none' x='0' y='0'>0</text>");
		int i;
		for (i = 0; (i+1)*pot < d_max; i++) {
			printf ("<path transform='translate(%g,0)' "
				"style='stroke:black;fill:none' d='M 0 %d l %g 0 l 0 %d'/>",
				i*scale_length, vsep+big_tick_height,
				scale_length, -big_tick_height);
			printf ("<text transform='translate(%g,0)' "
				"style='font-size:small;text-anchor:end;"
				"ststroke:black' x='0' y='0'>%g</text>",
				(i+1)*scale_length, (i+1)*pot);
		}
	}
	printf ("<text style='font-size:small;stroke:none' x='0' y='%d'>"
		"%s</text>", units_text_voffset, branch_length_unit);
	printf ("</g>");
}

void display_svg_tree_orthogonal(struct rooted_tree *tree,
		struct h_data hd, int align_leaves, int with_scale_bar,
		char *branch_length_unit)
{
	double h_scale = -1;
	/* This could get more complicated if we print many trees with
	 * different scales. For now, it's fixed. */
	double v_scale = leaf_vskip;

	// if (css_map) set_clade_numbers(tree);
 	// prettify_labels(tree);

	if (0.0 == hd.d_max ) { hd.d_max = 1; } 	/* one-node trees */
	h_scale = (graph_width - hd.l_max - ROOT_SPACE - LBL_SPACE) / hd.d_max;

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
	if (with_scale_bar) draw_scale_bar(tree, h_scale, v_scale, hd.d_max,
			branch_length_unit);
	printf ("</g>");
}
