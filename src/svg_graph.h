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

#include <stdbool.h>

/* Interface of svg_graph.c: functions used for drawing SVG trees. */

enum graph_style { SVG_ORTHOGONAL, SVG_RADIAL };

/* Functions for displaying a tree as SVG. This is the visible interface for
 * client code such as display.c User should call any of the set_svg_*()
 * functions that are needed, then svg_init(), then svg_header(),
 * display_svg_tree(), and finally svg_footer(). */

struct rooted_tree;

/* The following are for setting parameters for the SVG job. */

void set_width(int);
void set_leaf_vskip(double);
void set_style(int);
void set_URL_map_file(FILE *);
void set_label_angle_correction(double);
void set_left_label_angle_correction(double);
void set_scalebar_zero_at_root(bool);

/* Call this before calling svg_header(), etc, but _after_ the set_svg*()
 * functions. It will launch the initializations liek reading the color map,
 * etc. Returns FAILURE if there was an error (which will be a malloc()
 * problem). */

int svg_init();

/* Writes an SVG header */

void svg_header(int nb_leaves, bool with_scale_bar, enum graph_style style);

/* Writes a tree into a <g> object. If 'align_leaves' is true, the leaves will
 * be aligned (use this for cladograms). If 'with_scale_bar' is true, a sclae
 * bar will be drawn. The string 'branch_length_unit' is used as a label in the
 * scale bar (e.g., "substitutions/site"). */

enum display_status display_svg_tree(struct rooted_tree *,
		enum graph_style style, bool align_leaves,
		bool with_scale_bar, char *branch_length_unit);

/* Writes an SVG footer */

void svg_footer();

/* Pass this as a callback to destroy_tree */

void destroy_svg_node_data(void *);
