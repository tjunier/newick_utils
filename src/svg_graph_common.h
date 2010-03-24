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

/* Declarations used by all SVG modules, but not intended for client code. */

#define ROOT_SPACE 10			/* pixels */
#define LBL_SPACE 10			/* pixels */
#define LBL_VOFFSET 4			/* pixels */
#define INNER_LBL_SPACE 4		/* pixels */
#define edge_length_v_offset -4 	/* pixels */
#define URL_MAP_SIZE 100		/* bins */
#define UNSTYLED_CLADE 0

extern struct hash *url_map;
extern char *leaf_label_class;
extern char *inner_label_class;
extern int graph_width;
extern int svg_whole_v_shift; 
extern double label_char_width;
extern int scale_bar_height;

/* rnode data for SVG trees */

// TODO: URLs should be passed through the node, too.

struct svg_data {
	double top;
	double bottom;
	double depth;
	int group_nb;	/* For attributing styles */
	char *ornament;	/* SVG decorations */
	/* ... other node properties ... */
};

/* defined in graph_common.h, as it is also used by client code. */
enum inner_lbl_pos;

/* The following are setters, their function should be obvious */

void set_svg_CSS_map_file(FILE *);
void set_svg_ornament_map_file(FILE *);
void set_svg_leaf_label_style(char *);
void set_svg_inner_label_style(char *);
void set_svg_inner_label_pos(enum inner_lbl_pos);
void set_svg_edge_label_style(char *);
void set_svg_plain_node_style(char *);
void set_svg_root_length(int);
void set_svg_label_char_width(double);

/* Returns the largest power of ten not greater than argument */

double largest_PoT_lte(double);

void draw_scale_bar(int, double, double, double, char *);

/* Pass this as a callback to destroy_tree_cb */

void destroy_svg_node_data(struct rnode *);
