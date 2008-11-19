#include <stdio.h>
#include <string.h>

#include "list.h"
#include "rnode.h"
#include "redge.h"
#include "tree.h"
#include "hash.h"
#include "node_pos_alloc.h"
#include "svg_graph_common.h"
#include "math.h"

const double PI = 3.14159;

/* If this is zero, the label's baseline is aligned on the branch. Use it to
 * nudge the labels a small angle. Unfortunately the correct amount will depend
 * on the graph's diameter and the label font size. */
static double svg_label_angle_correction = 0.0;

/* The following applies to labels on the left side of the tree (because they
 * are subject to a 180° rotation, see draw_text_radial(). The default value
 * below was determined by trial and error. */
static double svg_left_label_angle_correction = -0.0349; /* -2°, in radians */ 

static int svg_root_length = ROOT_SPACE;

void set_svg_label_angle_correction(double corr)
{
	svg_label_angle_correction = corr;
}

void set_svg_left_label_angle_correction(double corr)
{
	svg_left_label_angle_correction = corr;
}

void set_svg_root_length(int length)
{
	svg_root_length = length;
}

/* Outputs an SVG <g> element with all the tree branches, radial. In this
 * context, a node's 'top' and 'bottom' are angles, not vertical positions */

void draw_branches_radial (struct rooted_tree *tree, const double r_scale,
		const double a_scale, int align_leaves, double dmax)
{
	printf( "<g"
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

		int group_nb = node_data->group_nb;
		double svg_radius = svg_root_length + (r_scale * node_data->depth);
		double svg_top_angle = a_scale * node_data->top; 
		double svg_bottom_angle = a_scale * node_data->bottom; 
		double svg_mid_angle =
			0.5 * a_scale * (node_data->top+node_data->bottom);
		double svg_mid_x_pos = svg_radius * cos(svg_mid_angle);
		double svg_mid_y_pos = svg_radius * sin(svg_mid_angle);
		int large_arc_flag;
		if (svg_bottom_angle - svg_top_angle > PI)
			large_arc_flag = 1; /* keep 1 and 0: literal SVG flags */
		else
			large_arc_flag = 0;

		/* draw node (arc), except for leaves */
		if (! is_leaf(node)) {
			double svg_top_x_pos = svg_radius * cos(svg_top_angle);
			double svg_top_y_pos = svg_radius * sin(svg_top_angle);
			double svg_bot_x_pos = svg_radius * cos(svg_bottom_angle);
			double svg_bot_y_pos = svg_radius * sin(svg_bottom_angle);
			printf("<path class='clade_%d'"
			       " d='M%.4f,%.4f A%4f,%4f 0 %d 1 %.4f %.4f'/>",
				group_nb,
				svg_top_x_pos, svg_top_y_pos,
				svg_radius, svg_radius,
				large_arc_flag,
				svg_bot_x_pos, svg_bot_y_pos);
		}
		/* draw radial line */
		if (is_root(node)) {
			printf("<line x1='0' y1='0' x2='%.4f' y2='%.4f'/>",
				svg_mid_x_pos, svg_mid_y_pos);
		} else {
			struct rnode *parent = node->parent_edge->parent_node;
			struct svg_data *parent_data = parent->data;
			double svg_parent_radius = svg_root_length + (
				r_scale * parent_data->depth);
			double svg_par_x_pos = svg_parent_radius * cos(svg_mid_angle);
			double svg_par_y_pos = svg_parent_radius * sin(svg_mid_angle);
			printf ("<line class='clade_%d' "
				"x1='%.4f' y1='%.4f' x2='%.4f' y2='%.4f'/>",
				group_nb,
				svg_mid_x_pos, svg_mid_y_pos,
				svg_par_x_pos, svg_par_y_pos);
		}

	}
	printf("</g>");
}

/* Prints the node text (labels and lengths) in a <g> element, radial */

void draw_text_radial (struct rooted_tree *tree, const double r_scale,
		const double a_scale, int align_leaves, double dmax)
{
	printf( "<g style='stroke:none'>");

	struct list_elem *elem;
	for (elem=tree->nodes_in_order->head; NULL!=elem; elem=elem->next) {
		struct rnode *node = elem->data;
		struct svg_data *node_data = (struct svg_data *) node->data;

		/* For cladograms */
		if (align_leaves && is_leaf(node))
			node_data->depth = dmax;

		double radius = svg_root_length + (r_scale * node_data->depth);
		double mid_angle =
			0.5 * a_scale * (node_data->top+node_data->bottom);
		double x_pos;
		double y_pos;

		mid_angle += svg_label_angle_correction;

		double lbl_space;
		if (is_leaf(node))
			lbl_space = LBL_SPACE;
		else
			lbl_space = INNER_LBL_SPACE;

		char *url = NULL;
		if (url_map) url = hash_get(url_map, node->label);

		char *class;
		if (is_leaf(node))
			class = leaf_label_class;
		else
			class = inner_label_class;

		/* draw label IFF it is nonempty */
		if (0 != strcmp(node->label, "")) {
			if (url) printf ("<a %s>", url);
			if (cos(mid_angle) >= 0)  {
				x_pos = (radius+lbl_space) * cos(mid_angle);
				y_pos = (radius+lbl_space) * sin(mid_angle);
				printf("<text class='%s' "
				       "transform='rotate(%g,%g,%g)' "
				       "x='%.4f' y='%.4f'>%s</text>",
					class,
					mid_angle / (2*PI) * 360,
					x_pos, y_pos,
					x_pos, y_pos, node->label);
			}
			else {
				mid_angle += svg_left_label_angle_correction;
				x_pos = (radius+lbl_space) * cos(mid_angle);
				y_pos = (radius+lbl_space) * sin(mid_angle);
				printf(	"<text class='%s' "
					"style='text-anchor:end;' "
				       	"transform='rotate(%g,%g,%g) rotate(180,%g,%g)' "
				       "x='%.4f' y='%.4f'>%s</text>",
					class,
					mid_angle / (2*PI) * 360,
					x_pos, y_pos,
					x_pos, y_pos,
					x_pos, y_pos, node->label);
			}
			if (url) printf("</a>");
		}
		/* TODO: add this when node labels work */
		/*
		if (! is_root(node)) {
			struct rnode *parent = node->parent_edge->parent_node;
			struct svg_data *parent_data = parent->data;
			double svg_parent_h_pos = svg_root_length + (
				h_scale * parent_data->depth);
				*/
			/* Print branch length IFF it is nonempty AND 
			 * requested size is not 0 */
			/*
			if (0 != strcmp(branch_length_font_size, "0") &&
			    0 != strcmp(node->parent_edge->length_as_string, "")) {
				printf("<text style='stroke:none;font-size:%s' "
					"x='%4f' y='%4f'>%s</text>",
					branch_length_font_size,
					(svg_h_pos + svg_parent_h_pos) / 2.0,
					edge_length_v_offset + svg_mid_pos,
					node->parent_edge->length_as_string);
			}
		}
		*/
	}
	printf("</g>");
}

void display_svg_tree_radial(struct rooted_tree *tree,
		struct h_data hd, int align_leaves)
{
	double r_scale = -1;
	/* By using 1.9 PI instead of 2 PI, we leave a wedge that shows the
	 * tree's bounds */
	double a_scale = 1.9 * PI / leaf_count(tree); /* radians */

	if (0.0 == hd.d_max ) { hd.d_max = 1; } 	/* one-node trees */
	/* draw nodes */
	r_scale = (0.5 * graph_width - CHAR_WIDTH * hd.l_max - 2 * svg_root_length - LBL_SPACE) / hd.d_max;
	printf( "<g transform='translate(%g,%g)'>",
		 	graph_width / 2.0, graph_width / 2.0);
	/* We draw all the tree's branches in an SVG group of their own, to
	 * facilitate editing. */
	draw_branches_radial(tree, r_scale, a_scale, align_leaves, hd.d_max);
	/* likewise for text */
	draw_text_radial(tree, r_scale, a_scale, align_leaves, hd.d_max);
	printf ("</g>");
}
