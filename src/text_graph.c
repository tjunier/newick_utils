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
/* text_graph.c - functions for drawing trees on a text canvas. */

#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <stdbool.h>
#include <string.h>

#include "canvas.h"
#include "tree.h"
#include "list.h"
#include "simple_node_pos.h"
#include "rnode.h"
#include "node_pos_alloc.h"
#include "common.h"
#include "graph_common.h"
#include "masprintf.h"

static const int LBL_SPACE = 2;
static const int ROOT_SPACE = 1;
static const int SCALEBAR_SPACE = 4;

/* Writes the nodes to the canvas. Assumes that the edges have been
 * attributed a double value in field 'length' (in this case, it is done in
 * set_node_depth()). */

void draw_tree(struct canvas *canvas, struct rooted_tree *tree,
		const double scale, int align_leaves, double dmax,
		enum inner_lbl_pos inner_label_pos)
{
	struct list_elem *elem;

	for (elem = tree->nodes_in_order->head;
			NULL != elem; elem = elem->next) {
		struct rnode *node =  elem->data;
		struct simple_node_pos *pos =  node->data;
		/* For cladograms */
		if (align_leaves && is_leaf(node))
			pos->depth = dmax;

		int h_pos = rint(ROOT_SPACE + (scale * pos->depth));
		int top = rint(2*pos->top);
		int bottom = rint(2*pos->bottom);
		int mid = rint(pos->top+pos->bottom);	/* (2*top + 2*bottom) / 2 */
		/* draw node */
		canvas_draw_vline(canvas, h_pos, top, bottom);
		if (is_root(node)) {
			canvas_write(canvas, 0, mid, "=");
		} else {
			struct simple_node_pos *parent_data = node->parent->data;
			int parent_h_pos = rint(ROOT_SPACE + (scale * parent_data->depth));
			canvas_draw_hline(canvas, mid, parent_h_pos, h_pos);
		}

		/* Don't bother printing label if it is "" */
		if (strcmp(node->label, "") == 0)
			continue;

		/* print label */
		if (is_inner_node(node)) {
			double parent_depth = (
				(struct simple_node_pos *)
					node->parent->data)->depth;
			switch (inner_label_pos) {
			case INNER_LBL_LEAVES:
				/* rint() rounds halfay, better than automatic
				 * double->int conversion */
				h_pos = rint(ROOT_SPACE + (scale * pos->depth));
				h_pos += LBL_SPACE;
				break;
			case INNER_LBL_MIDDLE:
				h_pos = rint(ROOT_SPACE + (scale *
					(parent_depth + pos->depth) / 2));
				break;
			case INNER_LBL_ROOT:
				h_pos = rint(ROOT_SPACE + (scale *
					parent_depth));
				h_pos += LBL_SPACE;
				break;
			default:
				assert(0);
			}
		} else {
			// Root or leaves
			h_pos += LBL_SPACE;
		}
		canvas_write(canvas, h_pos, mid, node->label);
	}
}

void draw_scalebar(struct canvas *canvas, const double scale,
		const double dmax, char *units, bool scale_zero_at_root)
{
	int v_pos = get_canvas_height(canvas) - SCALEBAR_SPACE;
	int h_start = ROOT_SPACE;
	int h_end = ROOT_SPACE + rint(scale * dmax);
	canvas_draw_hline(canvas, v_pos, h_start, h_end);
	float interval = tick_interval(dmax);
	if (scale_zero_at_root) {
		float x = 0;
		while (x <= dmax) {
			int tick_h_pos = ROOT_SPACE + rint(scale * x);
			canvas_write(canvas, tick_h_pos, v_pos, "|");
			char *tick_lbl = masprintf("%g", x);
			int tick_lbl_len = strlen(tick_lbl);
			int tick_lbl_pos = tick_h_pos - tick_lbl_len + 1;
			if (tick_lbl_pos < ROOT_SPACE)
				tick_lbl_pos = ROOT_SPACE;
			canvas_write(canvas, tick_lbl_pos, v_pos + 1, tick_lbl);
			free (tick_lbl);
			x += interval;
			if (dmax == 0)
				break;
		}
	} else {
		/* scale zero at max depth */
		float x = dmax;
		while (x >= 0) {
			int tick_h_pos = ROOT_SPACE + rint(scale * x);
			canvas_write(canvas, tick_h_pos, v_pos, "|");
			char *tick_lbl = masprintf("%g", dmax - x);
			int tick_lbl_len = strlen(tick_lbl);
			int tick_lbl_pos = tick_h_pos - tick_lbl_len + 1;
			if (tick_lbl_pos < ROOT_SPACE)
				tick_lbl_pos = ROOT_SPACE;
			canvas_write(canvas, tick_lbl_pos,
					v_pos + 1, tick_lbl);
			free (tick_lbl);
			x -= interval;
		}
	}
	canvas_write(canvas, h_start, v_pos + 2, units);

}

/* We create a new canvas every time - we could reuse one, but this would then
have to be passed, increasing coupling and diminishing implementation hiding.
What's more, we can't assume that the new tree will fit in the old canvas. */

enum display_status display_tree(
		struct rooted_tree *tree,
		int width,
		int align_leaves,
		enum inner_lbl_pos inner_label_pos,
		bool with_scalebar,
		char *branch_length_units,
		bool scale_zero_at_root)
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
	if (FAILURE == hd.status) return DISPLAY_MEM_ERROR;
	double scale = -1;
	struct canvas *canvasp;

	int scalebar_space = 0;
	if (with_scalebar)
		scalebar_space = SCALEBAR_SPACE;
	/* create canvas and draw nodes on it */
	scale = (width - hd.l_max - ROOT_SPACE - LBL_SPACE) / hd.d_max;
	if (0.0 == hd.d_max ) { scale = 1; } 	/* one-node trees */
	canvasp = create_canvas(width, 2 * num_leaves + scalebar_space);
	draw_tree(canvasp, tree, scale, align_leaves, hd.d_max,
			inner_label_pos);
	if (with_scalebar)
		draw_scalebar(canvasp, scale, hd.d_max, branch_length_units,
				scale_zero_at_root);

	
	/* output */
	canvas_dump(canvasp);

	/* release memory */
	destroy_canvas(canvasp);

	return DISPLAY_OK;
}
