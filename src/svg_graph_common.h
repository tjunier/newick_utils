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

/* Declarations used by SVG code, but not restricted to a single module. */

#define ROOT_SPACE 10			/* pixels */
#define LBL_VOFFSET 4			/* pixels */
#define INNER_LBL_SPACE 4		/* pixels */
#define edge_length_v_offset -4 	/* pixels */
#define URL_MAP_SIZE 100		/* bins */
#define UNSTYLED_CLADE 0

extern struct hash *url_map;
extern char *leaf_label_class;
extern char *inner_label_class;
extern int graph_width;
extern double label_char_width;
extern int scale_bar_height;
extern int label_space;

/* rnode data for SVG trees */

/* URLs could be passed through the node, too; but AFAIK clickable trees are
 * relatively rarely used, so if we add a member to this structure it will most
 * often not be used. And even then, it is not clear that all nodes in the tree
 * will have URLs. So the current map-based approach seems best here. */

/** A structure for node data pertaining to SVG. Structs of this type are
 * pointed to by struct rnode's \a data member. */

struct svg_data {
	double top;	/**< top of node (ortho) or leftmost angle (radial) */
	double bottom;	/**< bottom of node (ortho) or rightmost most angle (radial) */
	double depth;	/**< depth in tree */
	int group_nb;	/**< For attributing styles */
	char *ornament;	/**< SVG decorations */
	/* ... other node properties ... */
};

/* defined in graph_common.h, as it is also used by client code. */
enum inner_lbl_pos;

/* The following are setters, their function should be obvious */

void set_clade_CSS_map_file(FILE *);
void set_ornament_map_file(FILE *);
void set_leaf_label_style(char *);
void set_inner_label_style(char *);
void set_inner_label_pos(enum inner_lbl_pos);
void set_edge_label_style(char *);
void set_plain_node_style(char *);
void set_root_length(int);
void set_label_char_width(double);
void add_to_label_space(int);

/* Returns the largest power of ten not greater than argument */

double largest_PoT_lte(double);

void draw_scale_bar(double, double, double, double, char *);

