/* 

Copyright (c) 2009 Thomas Junier and Evgeny Zdobnov, University of Geneva
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice,
    this list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright notice,
    this list of conditions and the following disclaimer in the documentation
    and/or other materials provided with the distribution.
* Neither the name of the University of Geneva nor the names of its
    contributors may be used to endorse or promote products derived from this
    software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>

#include "list.h"
#include "rnode.h"
#include "tree.h"
#include "hash.h"
#include "node_pos_alloc.h"
#include "graph_common.h"
#include "svg_graph_common.h"
#include "math.h"

extern enum inner_lbl_pos inner_label_pos;

const double PI = 3.14159;
const int Scale_bar_left_space = 10;
const int NUDGE_DISTANCE = 3;	/* px */

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
			struct svg_data *parent_data = node->parent->data;
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
		/* draw ornament, if any */
		if (NULL != node_data->ornament)
			printf ("<g transform='translate(%.4f,%.4f)'>%s</g>",
					svg_mid_x_pos, svg_mid_y_pos,
					node_data->ornament);

	}
	printf("</g>");
}

/* Draws a node label */

static void draw_label(struct rnode *node, double radius,
		double mid_angle, const double r_scale,
		const char *class, const char *url)
{
	double x_pos;
	double y_pos;
	/* Will set this to true when the label must be drawn parallel to the
	 * branch, rather than on the exact same line */
	bool nudge = false;

	if (url) printf ("<a %s>", url);

	if (is_inner_node(node)) {
		struct svg_data *parent_data = NULL;
		double parent_radius = -1;
		switch (inner_label_pos) {
			case INNER_LBL_LEAVES:
				break;
			case INNER_LBL_MIDDLE:
				parent_data = node->parent->data;
				parent_radius = svg_root_length + (
					r_scale * parent_data->depth);
				radius = 0.5 * (radius + parent_radius);
				nudge = true;
				break;
			case INNER_LBL_ROOT:
				parent_data = node->parent->data;
				parent_radius = svg_root_length + (
					r_scale * parent_data->depth);
				radius = parent_radius;
				nudge = true;
				break;
			default:
				assert(0);
		}
	}

	if (cos(mid_angle) >= 0)  {
		x_pos = radius * cos(mid_angle);
		y_pos = radius * sin(mid_angle);
		if (nudge) {
			x_pos -= (NUDGE_DISTANCE * cos(mid_angle + PI / 2));
			y_pos -= (NUDGE_DISTANCE * sin(mid_angle + PI / 2));
		}
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
		x_pos = radius * cos(mid_angle);
		y_pos = radius * sin(mid_angle);
		if (nudge) {
			x_pos += (NUDGE_DISTANCE * cos(mid_angle + PI / 2));
			y_pos += (NUDGE_DISTANCE * sin(mid_angle + PI / 2));
		}
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

		mid_angle += svg_label_angle_correction;

		if (is_leaf(node))
			radius += LBL_SPACE;
		else
			radius += INNER_LBL_SPACE;

		char *url = NULL;
		if (url_map) url = hash_get(url_map, node->label);

		char *class;
		if (is_leaf(node))
			class = leaf_label_class;
		else
			class = inner_label_class;

		/* draw label IFF it is nonempty */
		if (0 != strcmp(node->label, ""))
			draw_label(node, radius, mid_angle, r_scale,
					class, url);
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

/* Prints an SVG comment block with some run parameters. Useful for debugging */

void params_as_svg_comment (struct h_data hd, double node_area_width,
		double r_scale)
{
	printf ("<!-- SVG parameters:\n"
		"    primary params:\n"
		"    ..............\n"
		"    graph width [g]:\t\t%4d px\n"
		"    label char width [c]:\t%4g px\n"
		"    max label length [l]:\t%4d characters\n"
		"    root length [r]:\t\t%4d px\n"
		"    label space [s]:\t\t%4d px\n"
		"    tree depth [d]:\t\t%4g arbitrary units\n\n"
		"    derived params:\n"
		"    ..............\n"
		"    label area [cl]:\t\t\t%4g px\n"
		"    node area [n = g/2 - cl - r - s]:\t%4g px\n"
		"    radial scale [S = n / d]:\t\t%4g px / arbitrary units\n"
		"  -->\n",
		graph_width,
	       	label_char_width,
	       	hd.l_max,
		svg_root_length,
	       	LBL_SPACE,
		hd.d_max,
	       	label_char_width * hd.l_max,
		node_area_width,
		r_scale);
}

void display_svg_tree_radial(struct rooted_tree *tree,
		struct h_data hd, int align_leaves, int with_scale_bar,
		char *branch_length_unit)
{
	double r_scale = -1;
	/* By using 1.9 PI instead of 2 PI, we leave a wedge that shows the
	 * tree's bounds */
	double a_scale = 1.9 * PI / leaf_count(tree); /* radians */

	if (0.0 == hd.d_max ) { hd.d_max = 1; } 	/* one-node trees */

	/* TODO: why twice svg_root_length? */
	double node_area_width = 0.5 * graph_width
			- label_char_width * hd.l_max
			- svg_root_length - LBL_SPACE;
	r_scale = node_area_width / hd.d_max;

	// params_as_svg_comment(hd, node_area_width, r_scale);

	/* Given that we only 'use' 1.9 PI instead of 2 PI (see above), we can
	 * rotate the whole graph by 0.05 PI so that we can draw the scale bar
	 * on the horizontal axis. */
	/* NOTE: I've tried drawing the scale bar from the tree's center. It's
	 * not as good as it seems, so for now I shall draw it in the lower
	 * left corner, just like in an orthogonal tree. If someone really
	 * insists on this, it can be reenabled by commenting out the following
	 * (and commenting the appropriate parts), and drwaing the scale bar at
	 * (0, 0) (actually, needs a bit of vertical nudging, see
	 * draw_scale_bar() */
	
	// double rotation = 0.1 * PI / (2 * PI) * 360;	/* degrees */
	/*
	printf( "<g transform='translate(%g,%g) rotate(%g)'>",
		 	graph_width / 2.0, graph_width / 2.0, rotation);
	*/

	printf( "<g transform='translate(%g,%g)'>", graph_width / 2.0,
			graph_width / 2.0); 
	/* We draw all the tree's branches in an SVG group of their own, to
	 * facilitate editing. */
	draw_branches_radial(tree, r_scale, a_scale, align_leaves, hd.d_max);
	/* likewise for text */
	draw_text_radial(tree, r_scale, a_scale, align_leaves, hd.d_max);
	printf ("</g>");
	if (with_scale_bar)
		draw_scale_bar(Scale_bar_left_space, (double) graph_width,
				r_scale, hd.d_max, branch_length_unit);
}
