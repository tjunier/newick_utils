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

#include "tree.h"
#include "list.h"
#include "rnode.h"
#include "hash.h"
#include "node_pos_alloc.h"
#include "graph_common.h"
#include "svg_graph_common.h"

extern enum inner_lbl_pos inner_label_pos;
static const int whole_v_shift = 20; 	/* Vertical translation of whole graph */

/* Prevents labels of single-child nodes from being crossed over by the child
 * branch. Also used when putting inner labels near root. */
const int KNEE_NODE_V_NUDGE = 6;
const int INNER_LBL_H_NUDGE = 2;

double leaf_vskip = -1; 	/* Vertical separation of leaves (px) */

void set_leaf_vskip(double skip) { leaf_vskip = skip; }

/* Overall graph height (assuming 1 tree per graph, as always) */

int graph_height(int nb_leaves, int with_scale_bar)
{
	int height = 2 * whole_v_shift + ((nb_leaves -1)  * leaf_vskip);
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
			struct svg_data *parent_data = node->parent->data;
			double svg_parent_h_pos = ROOT_SPACE + (
				h_scale * parent_data->depth);
			printf ("<line class='clade_%d' "
				"x1='%.4f' y1='%.4f' x2='%.4f' y2='%.4f'/>",
				node_data->group_nb, svg_parent_h_pos,
				 svg_mid_pos, svg_h_pos, svg_mid_pos);
		}
		/* draw ornament, if any */
		if (NULL != node_data->ornament) {
			/* ornament is considered text IFF it starts
			 * with "<text" */
			if (strstr(node_data->ornament, "<text") == 
					node_data->ornament) {
				printf ("<g style='text-anchor:end;"
					"stroke:none;fill:black'"
					" transform='"
					"translate(%g,%g)'>%s</g>",
					svg_h_pos,
					svg_mid_pos + edge_length_v_offset,
					node_data->ornament);
			} else { /* not text */
				printf ("<g transform='"
					"translate(%g,%g)'>%s</g>",
						svg_h_pos, svg_mid_pos,
						node_data->ornament);
			}

		}
	}
	printf("</g>");
}

/* Draws a node label */

void draw_label(const double svg_h_pos, const double svg_mid_pos,
		const double h_scale, struct rnode *node,
		const char *class, const char *url)
{
	/* Defaults: use node's position */
	double h_pos = svg_h_pos + label_space;
	double v_pos = svg_mid_pos + LBL_VOFFSET; 

	if (url) printf ("<a %s>", url);

	bool v_nudge = false;	/* set to true if label across branch */

	/* inner node label can be positioned in various ways */
	if (is_inner_node(node)) {
		struct svg_data *parent_data = NULL;
		double svg_parent_h_pos = -1;
		switch (inner_label_pos) {
			case INNER_LBL_LEAVES:
				/* just use defaults */
				break;
			case INNER_LBL_MIDDLE:
				parent_data = node->parent->data;
				svg_parent_h_pos = ROOT_SPACE + (
					h_scale * parent_data->depth);
				h_pos = 0.5 * (svg_h_pos + svg_parent_h_pos);
				v_nudge = true;
				break;
			case INNER_LBL_ROOT:
				/* label near root */
				parent_data = node->parent->data;
				svg_parent_h_pos = ROOT_SPACE + (
					h_scale * parent_data->depth);
				h_pos = svg_parent_h_pos + INNER_LBL_H_NUDGE;;
				v_nudge = true;
				break;
			default:
				assert(0);
		}
	}

	if (1 == children_count(node))
		v_nudge = true;

	if (v_nudge)
		v_pos -= KNEE_NODE_V_NUDGE;

	printf("<text class='%s' "
	       "x='%.4f' y='%.4f'>%s</text>",
		class, h_pos, v_pos, node->label);

	if (url) printf ("</a>");
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
			draw_label(svg_h_pos, svg_mid_pos, h_scale,
					node, class, url);

		/* Branch lengths */

		if (! is_root(node)) {
			struct svg_data *parent_data = node->parent->data;
			double svg_parent_h_pos = ROOT_SPACE + (
				h_scale * parent_data->depth);
			/* Print branch length IFF it is nonempty AND 
			 * requested size is not 0 */
			if (0 != strcmp(node->edge_length_as_string,
						"")) {
				printf("<text class='edge-label' "
					"x='%4f' y='%4f'>%s</text>",
					(svg_h_pos + svg_parent_h_pos) / 2.0,
					edge_length_v_offset + svg_mid_pos,
					node->edge_length_as_string);
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
			- label_space
		) / hd.d_max;

	/* Tree is in a separate group - may be useful when (if?) there are
	 * more than one tree*/
	printf( "<g"
		" transform='translate(0,%d)'"
		">", whole_v_shift);
	/* We draw all the tree's branches in an SVG group of their own, to
	 * facilitate editing via Inkscape, Illustrator, etc. */
	draw_branches_ortho(tree, h_scale, v_scale, align_leaves, hd.d_max);
	/* ... likewise for text */
	draw_text_ortho(tree, h_scale, v_scale, align_leaves, hd.d_max);
	/* Draw scale bar if required */
	if (with_scale_bar) {
		double scalebar_vpos = (double) graph_height(leaf_count(tree),
				with_scale_bar) - whole_v_shift;
		draw_scale_bar(ROOT_SPACE, scalebar_vpos, h_scale, hd.d_max,
			branch_length_unit);
	}
	printf ("</g>");
}
