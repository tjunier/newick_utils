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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <ctype.h>
#include <stdbool.h>

#include "common.h"
#include "graph_common.h"
#include "hash.h"
#include "lca.h"
#include "list.h"
#include "masprintf.h"
#include "node_pos_alloc.h"
#include "nodemap.h"
#include "readline.h"
#include "rnode.h"
#include "svg_graph_common.h"
#include "svg_graph.h"
#include "svg_graph_ortho.h"
#include "svg_graph_radial.h"
#include "tree.h"
#include "xml_utils.h"
#include "error.h"

enum { INDIVIDUAL, CLADE, UNKNOWN };

struct css_map_element {
	int group_type;	/* INDIVIDUAL or CLADE */
	int group_nb;	
	char *style;	/* a CSS specification */
	struct llist *labels;
};

struct ornament_map_element {
	int group_type;	/* INDIVIDUAL or CLADE */
	bool text;
	char *ornament;	/* an SVG snippet */
	struct llist *labels;
};

char *leaf_label_class = "leaf-label";
char *inner_label_class = "inner-label";

static bool init_done = false;

/************************** external variables *****************************/

/* We can't pass all the parameters to display_svg_tree() or any other function
 * - there are too many of them - so we use external variables. The static ones
 *   are only used here, the other ones have external linkage because they are
 *   used in other modules. */

static FILE *url_map_file = NULL;
static FILE *clade_css_map_file = NULL;
static FILE *ornament_map_file = NULL;
static char *leaf_label_style = NULL;
static char *inner_label_style = NULL;
static char *edge_label_style = NULL;
static char *plain_node_style = NULL;
static struct llist *css_map = NULL;
static struct llist *ornament_map = NULL;

int scale_bar_height = 30;	/* px */
struct hash *url_map = NULL;
int graph_width = -1;
double label_char_width = -1;
enum inner_lbl_pos inner_label_pos = -1;
bool scalebar_zero_at_root = true;
int label_space = 10;

/* These are setters for the external variables. This way I can keep most of
 * them static. I just don't like variables open to anyone, maybe I did too
 * much OO... I also provide setters for the nonstatic ones, so the interface
 * is more homogeneous. */

void set_width(int width) { graph_width = width; }
void set_URL_map_file(FILE * map) { url_map_file = map; }
void set_clade_CSS_map_file(FILE * map) { clade_css_map_file = map; }
void set_ornament_map_file(FILE * map) { ornament_map_file = map; }
void set_leaf_label_style(char *style) { leaf_label_style = style; }
void set_inner_label_style(char *style) { inner_label_style = style; }
void set_inner_label_pos(enum inner_lbl_pos pos) { inner_label_pos = pos; }
void set_edge_label_style(char *style) { edge_label_style = style; }
void set_plain_node_style(char *style) { plain_node_style = style; }
void set_label_char_width(double width) { label_char_width = width; }
void set_scalebar_zero_at_root(bool at_root) { scalebar_zero_at_root = at_root; }
void add_to_label_space(int correction) { label_space += correction; }

/************************** functions *****************************/

static void svg_CSS_stylesheet()
{
	struct list_elem *el;

	printf ("<defs><style type='text/css'><![CDATA[\n");
	if (NULL != plain_node_style)
		printf (" .clade_0 {%s}\n", plain_node_style);
	if (css_map) {
		for (el = css_map->head; NULL != el; el = el->next) {
			struct css_map_element *css_el = el->data;
			printf(" .clade_%d {%s}\n", css_el->group_nb,
					css_el->style);
		}
	}
	printf (" .leaf-label {%s}\n", leaf_label_style);
	printf (" .inner-label {%s}\n", inner_label_style);
	printf (" .edge-label {%s}\n", edge_label_style);
	printf ("]]></style></defs>");
}


/* A helper function for read_css_map(). Given a 'type' string (read from the
 * CSS style file), returns the appropriate type as an integer, or UNKNOWN if
 * the type is unknown. */

static int get_group_type(const char *type)
{
	/* Make lower-case local copy of 'type' */
	char *t = strdup(type);
	int l = strlen(t);
	int i;
	for (i = 0; i < l; i++)
		t[i] = tolower(t[i]);

	/* Instead of looking for exact matches, look for any prefix of "clade"
	 * or "individual": this allows the user to use full keywords like
	 * "CLADE", but also to abbreviate to "ind" or even "I" or "C". */
	char *clade = "clade";
	char *indiv = "individual";
	int result = UNKNOWN;
	if (strstr(clade, t) == clade) 
		result = CLADE;
	else if (strstr(indiv, t) == indiv)
		result = INDIVIDUAL;
	
	free(t);

	return result;
}

/* Builds a CSS map structure. This is a list of struct css_map_element, each
 * of which contains a group type (CLADE or INDIVIDUAL), a style specification,
 * and a list of labels. The style will apply to the clade defined by the
 * labels (if the type is CLADE) or to all individual nodes in the list (if the
 * style is INDIVIDUAL). */

static struct llist *read_css_map()
{
	/* Most errors are memory errors (but see below) */
	set_last_error_code(ERR_NOMEM);
	struct llist *css_map = create_llist();
	if (NULL == css_map) return NULL;

	char *line;
	int i = 1;
	while ((line = read_line(clade_css_map_file)) != NULL) {
		/* Skip comments and lines that are empty or all whitespace */
		if ('#' == line[0] || is_all_whitespace(line)) {
			free(line);
			continue;
		}

		struct css_map_element *css_el = malloc(
				sizeof(struct css_map_element));
		if (NULL == css_el) return NULL;

		/* Split line into whitespace-separated "words" (or words
		 * delimited by double quotes). First word is the CSS
		 * specification, second is group type, then come the labels.
		 * */
		struct llist *label_list = create_llist();
		if (NULL == label_list) return NULL;
		struct word_tokenizer *wtok = create_word_tokenizer(line);
		if (NULL == wtok) return NULL;
		/* Next errors are syntax errors */
		set_last_error_code(ERR_CSS_MAP_SYNTAX);
		char *style = wt_next_noquote(wtok);
		if (NULL == style) return NULL; 
		char *type = wt_next(wtok);
		if (NULL == type) return NULL;
		/* Errors are memory again */
		set_last_error_code(ERR_NOMEM);
		char *label;
		while ((label = wt_next(wtok)) != NULL) {
			if (! append_element(label_list, label)) return NULL;
		}
		destroy_word_tokenizer(wtok);
		free(line);
		/* It is a syntax error if there was no label */
		if (0 == label_list->count) {
				set_last_error_code(ERR_CSS_MAP_SYNTAX);
				return NULL;
		}
		css_el->group_type = get_group_type(type); 
		if (UNKNOWN == css_el->group_type) {
			fprintf (stderr, "WARNING: unknown group type '%s' (ignored)\n", type);
			free(css_el);
			free(type);
			destroy_llist(label_list);
			continue;
		}
		css_el->style = style;
		css_el->labels = label_list;
		css_el->group_nb = i;
		if (! append_element(css_map, css_el)) return NULL;
		i++;

		free(type);
	}

	fclose(clade_css_map_file);

	switch (read_line_status) {
		case READLINE_EOF:
			return css_map;
		case READLINE_ERROR:
			return NULL;
		default:
			assert(0);	/* programmer error */
	}
}

/* Builds an ornament map structure. This is like the CSS map, but for
 * ornaments. Also returns NULL on error. */

static struct llist *read_ornament_map()
{
	/* As for read_css_map() */
	set_last_error_code(ERR_NOMEM);
	struct llist *ornament_map = create_llist();
	if (NULL == ornament_map) return NULL;

	char *line;
	while ((line = read_line(ornament_map_file)) != NULL) {
		/* Skip comments and lines that are empty or all whitespace */
		if ('#' == line[0] || is_all_whitespace(line)) {
			free(line);
			continue;
		}

		struct ornament_map_element *oel = malloc(
				sizeof(struct ornament_map_element));
		if (NULL == oel) return NULL;

		/* Split line into whitespace-separated "words" (or words
		 * delimited by double quotes). First word is the ornament ,
		 * second is group type, then come the labels.  */

		struct llist *label_list = create_llist();
		if (NULL == label_list) return NULL;
		struct word_tokenizer *wtok = create_word_tokenizer(line);
		if (NULL == wtok) return NULL;
		/* Next errors are syntax errors */
		set_last_error_code(ERR_ORN_MAP_SYNTAX);
		char *ornament = wt_next_noquote(wtok);
		if (NULL == ornament) return NULL;
		char *type = wt_next(wtok);
		if (NULL == type) return NULL;
		/* Errors are memory again */
		set_last_error_code(ERR_NOMEM);
		char *label;
		while ((label = wt_next(wtok)) != NULL) {
			if (! append_element(label_list, label)) return NULL;
		}
		destroy_word_tokenizer(wtok);
		free(line);
		/* It is a syntax error if there was no label */
		if (0 == label_list->count) {
				set_last_error_code(ERR_ORN_MAP_SYNTAX);
				return NULL;
		}
		oel->group_type = get_group_type(type); 
		if (UNKNOWN == oel->group_type) {
			fprintf (stderr, "WARNING: unknown group type '%s' (ignored)\n", type);
			free(oel);
			free(type);
			destroy_llist(label_list);
			continue;
		}
		oel->ornament = ornament;
		/* ornament is considered text IFF it begins with '<text' */
		if (strstr(ornament, "<text") == ornament)
			oel->text = true;
		else
			oel->text = false;
		oel->labels = label_list;
		if (! append_element(ornament_map, oel))
			return NULL;

		free(type);
	}

	fclose(ornament_map_file);

	switch (read_line_status) {
		case READLINE_EOF:
			return ornament_map;
		case READLINE_ERROR:
			return NULL;
		default:
			assert(0);	/* programmer error */
	}
}

/* Reads in the URL map (label -> URL), returns NULL on error. */

static struct hash *read_url_map()
{
	/* As for read_css_map() */
	set_last_error_code(ERR_NOMEM);
	struct hash *url_map = create_hash(URL_MAP_SIZE);
	if (NULL == url_map) return NULL;

	char *line;
	while ((line = read_line(url_map_file)) != NULL) {
		/* Skip comments and lines that are empty or all whitespace */
		if ('#' == line[0] || is_all_whitespace(line)) {
			free(line);
			continue;
		}

		struct word_tokenizer *wtok = create_word_tokenizer(line);
		if (NULL == wtok) return NULL;
		/* Next errors are syntax errors */
		set_last_error_code(ERR_URL_MAP_SYNTAX);
		char *label = wt_next(wtok);
		if (NULL == label) return NULL;
		underscores2spaces(label);
		remove_quotes(label);
		char *url = wt_next(wtok);
		if (NULL == url) return NULL;
		char *escaped_url = escape_predefined_character_entities(url);
		char *anchor_attributes;
		anchor_attributes = masprintf("xlink:href='%s' ", escaped_url);
		/* Next errors are memory again */
		set_last_error_code(ERR_NOMEM);
		if (NULL == anchor_attributes) return NULL;
		char *att;
		while ((att = wt_next(wtok)) != NULL) {
			int att_len = strlen(anchor_attributes);
			/* add length of new attribute */
			att_len += strlen(att);
			att_len += 1;	/* trailing space */
			att_len += 1;	/* terminal '\0' */
			anchor_attributes = realloc(anchor_attributes,
				att_len * sizeof(char));
			if (NULL == anchor_attributes) return NULL;
			strcat(anchor_attributes, att);
			strcat(anchor_attributes, " ");

		}
		if (! hash_set(url_map, label, anchor_attributes))
			return NULL;
		destroy_word_tokenizer(wtok);
		free(line);
		free(label);
		free(url);
		free(escaped_url);
	}

	switch (read_line_status) {
		case READLINE_EOF:
			return url_map;
		case READLINE_ERROR:
			return NULL;
		default:
			assert(0);	/* programmer error */
	}	
}

/* Call this function before calling display_svg_tree(). Resist the temptation
 * to put it inside display_svg_tree(): it is kept separate because its job is
 * not directly to draw trees*/

/* Attributes group numbers to nodes, based on the CSS style map (if one was
 * supplied - see read_css_map() and svg_CSS_stylesheet() ). The group number
 * will translate directly into 'class' attributes in SVG, which in turn will
 * have a style defined according to the style map. */

// NOTE: this could be made more efficient by having two separate maps, one for
// CLADE elements and another for INDIVIDUAL elements. That way the list needs
// not be traversed twice. But all in all it will likely not make a big
// difference, so I'll keep it for later :-) 

static int set_group_numbers(struct rooted_tree *tree)
{
	struct list_elem *elem;
	struct css_map_element *css_el;

	/* Iterate through the CLADE style map elements. Each one contains
	 * (among others) a list of labels. Find the LCA of those labels (which
	 * can be matched by >1 node), and set its number to that of the css
	 * element. */

	for (elem = css_map->head; NULL != elem; elem = elem->next) {
		css_el = elem->data;
		if (CLADE != css_el->group_type) continue;
		struct llist *labels = css_el->labels;
		struct rnode *lca = lca_from_labels_multi(tree, labels);
		if (NULL == lca) {
			enum error_codes err = get_last_error_code();
			switch (err) {
			case ERR_NOMEM:
				return FAILURE;
			case ERR_NO_MATCHING_NODES:
				return SUCCESS;
			default:
				assert(0);	/* should not happen */
			}
		}

		struct svg_data *lca_data = lca->data;
		lca_data->group_nb = css_el->group_nb;
	}


	/* Now propagate the styles to the descendants */
	struct llist *nodes_in_reverse_order;
	nodes_in_reverse_order = llist_reverse(tree->nodes_in_order);
	if (NULL == nodes_in_reverse_order) return FAILURE;

	elem = nodes_in_reverse_order->head->next;	/* skip root */
	for (;  NULL != elem; elem = elem->next) {
		struct rnode *node = elem->data;
		struct svg_data *node_data = node->data;
		struct rnode *parent = node->parent;
		struct svg_data *parent_data = parent->data;
		/* Inherit parent node's style (clade number) IFF 
		    node has no style of its own */
		if (UNSTYLED_CLADE == node_data->group_nb) {
			node_data->group_nb = parent_data->group_nb;
		} 
				
	}
	destroy_llist(nodes_in_reverse_order);

	/* Now iterate through the INDIVIDUAL style map elements. They also
	 * contain a list of labels. Each label is matched by at least 1 node.
	 * All of these nodes get the map element's number (cf above, in which
	 * te LCA gets the number, which is then propagated to all descendants)
	 * */
	struct hash *map = create_label2node_list_map(tree->nodes_in_order);
	if (NULL == map) return FAILURE;
	for (elem = css_map->head; NULL != elem; elem = elem->next) {
		css_el = elem->data;
		if (INDIVIDUAL != css_el->group_type) continue;
		struct llist *labels = css_el->labels;
		struct llist *group_nodes = create_llist();
		if (NULL == group_nodes) return FAILURE; 
		/* Iterate over all labels of this element, adding the
		 * corresponding nodes to 'group_nodes' */
		struct list_elem *el;
		for (el = labels->head; NULL != el; el = el->next) {
			char *label = el->data;
			struct llist *nodes_of_label;
			nodes_of_label = hash_get(map, label);
			if (NULL == nodes_of_label) {
				fprintf (stderr, "WARNING: label '%s' "
						"not found - ignored.\n",
						label);
				continue;
			}
			struct llist *copy = shallow_copy(nodes_of_label);
			if (NULL == copy) return FAILURE;
			append_list(group_nodes, copy);
			free(copy); 	/* NOT destroy_llist(): the list
					   elements are in group_nodes. */
		}
		/* Set the group number for all nodes of this group */
		for (el = group_nodes->head; NULL != el; el = el->next)  {
			struct rnode *node = el->data;
			struct svg_data *node_data = node->data;
			node_data->group_nb = css_el->group_nb;
		}
		destroy_llist(group_nodes);
	}
	destroy_label2node_list_map(map);

	return SUCCESS;
}

/* Attributes ornaments to nodes, based on the ornament map (if one was
 * supplied - see read_ornament_map()). The ornament will be printed directly
 * at the node's position. */

// NOTE: this could be made more efficient by having two separate maps, one for
// CLADE elements and another for INDIVIDUAL elements. That way the list needs
// not be traversed twice. But all in all it will likely not make a big
// difference, so I'll keep it for later :-) 

static int set_ornaments(struct rooted_tree *tree)
{
	struct list_elem *elem;
	struct ornament_map_element *oel;

	/* Iterate through the CLADE style map elements. Each one contains
	 * (among others) a list of labels. Find the LCA of those labels (which
	 * can be matched by >1 node), and set its ornament */

	for (elem = ornament_map->head; NULL != elem; elem = elem->next) {
		oel = elem->data;
		if (CLADE != oel->group_type) continue;
		struct llist *labels = oel->labels;
		struct rnode *lca = lca_from_labels_multi(tree, labels);
		if (NULL == lca) return FAILURE;
		struct svg_data *lca_data = lca->data;
		lca_data->ornament = strdup(oel->ornament);
	}

	/* Now iterate through the INDIVIDUAL style map elements. They also
	 * contain a list of labels. Each label is matched by at least 1 node.
	 * All of these nodes get the ornament. */

	struct hash *map = create_label2node_list_map(tree->nodes_in_order);
	if (NULL == map) return FAILURE;
	for (elem = ornament_map->head; NULL != elem; elem = elem->next) {
		oel = elem->data;
		if (INDIVIDUAL != oel->group_type) continue;
		struct llist *labels = oel->labels;
		struct llist *group_nodes = create_llist();
		if (NULL == group_nodes) return FAILURE;
		/* Iterate over all labels of this element, adding the
		 * corresponding nodes to 'group_nodes' */
		struct list_elem *el;
		for (el = labels->head; NULL != el; el = el->next) {
			char *label = el->data;
			struct llist *nodes_of_label;
			nodes_of_label = hash_get(map, label);
			if (NULL == nodes_of_label) {
				fprintf (stderr, "WARNING: label '%s' "
						"not found - ignored.\n",
						label);
				continue;
			}
			struct llist *copy = shallow_copy(nodes_of_label);
			if (NULL == copy) return FAILURE;
			append_list(group_nodes, copy);
			free(copy); 	/* NOT destroy_llist(): the list
					   elements are in group_nodes. */
		}
		/* Set the ornament for all nodes of this group */
		for (el = group_nodes->head; NULL != el; el = el->next)  {
			struct rnode *node = el->data;
			struct svg_data *node_data = node->data;
			node_data->ornament = strdup(oel->ornament);
		}
		destroy_llist(group_nodes);
	}
	destroy_label2node_list_map(map);

	return SUCCESS;
}

/* Allocates and initializes an svg_data structure for each of the tree's
 * nodes. The real data are set later through callbacks (see
 * svg_set_node_top(), etc) */
/* Returns FAILURE IFF there is any problem (malloc(), in this case) */

static int svg_alloc_node_pos(struct rooted_tree *tree) 
{
	struct list_elem *le;
	struct rnode *node;

	for (le = tree->nodes_in_order->head; NULL != le; le = le->next) {
		node = le->data;
		struct svg_data *svgd = malloc(sizeof(struct svg_data));
		if (NULL == svgd) return FAILURE;
		svgd->top = svgd->bottom = svgd->depth = -1.0;
		svgd->group_nb = UNSTYLED_CLADE;	
		svgd->ornament = NULL;
		node->data = svgd;
	}
	return SUCCESS;
}

static void svg_set_node_top (struct rnode *node, double top)
{
	((struct svg_data *) node->data)->top = top;
}

static void svg_set_node_bottom (struct rnode *node, double bottom)
{
	((struct svg_data *) node->data)->bottom = bottom;
}

static double svg_get_node_top (struct rnode *node)
{
	return ((struct svg_data *) node->data)->top;
}

static double svg_get_node_bottom (struct rnode *node)
{
	return ((struct svg_data *) node->data)->bottom;
}

static void svg_set_node_depth (struct rnode *node, double depth)
{
	((struct svg_data *) node->data)->depth = depth;
}

static double svg_get_node_depth (struct rnode *node)
{
	return ((struct svg_data *) node->data)->depth;
}

double largest_PoT_lte(double arg)
{
	double l10 = log(arg) / log(10);
	double l10_fl = floor(l10);
	return pow(10, l10_fl);
}

/* Draws a square grid, centered on the orgin. This is for debugging. Anyway
 * some programs (like Eye of Gnome) just can't handle the grid. */

void draw_grid()
{
	int i,j;

	printf ("<g class='grid' style='stroke:grey'>");
	for (i = -1000; i <= 1000; i += 100)
		printf ("<path d='M %d -1000 v 2000'/>", i);
	for (j = -1000; j <= 1000; j += 100)
		printf ("<path d='M -1000 %d h 2000'/>", j);
	printf ("</g>");
}

int svg_init()
{
	if (NULL != clade_css_map_file) {
		css_map = read_css_map();
		if (NULL == css_map)
			return FAILURE;
	}
	if (NULL != ornament_map_file) {
		ornament_map = read_ornament_map();
		if (NULL == ornament_map)
			return FAILURE;
	}
	if (NULL != url_map_file) {
		url_map = read_url_map();
		if (NULL == url_map)
			return FAILURE;
	}
	init_done = 1;

	return SUCCESS;
}

/* Prints the SVG header. The first argument is the tree's number of leaves,
 * needed to compute height for orthogonal trees; the second argument is a
 * boolean that is true IFF we show a scale bar. */

void svg_header(int nb_leaves, bool with_scale_bar, enum graph_style style)
{
	int height;

	switch (style) {
		case SVG_ORTHOGONAL:
			/* image fits in a rectangle, so height != width */
			height = graph_height(nb_leaves, with_scale_bar);
			break;
		case SVG_RADIAL:
			/* image fits in a square, so height == width */
			height = graph_width;
			break;
		default:
			assert(0);
	}

	printf( "<?xml version='1.0' standalone='no'?>"
	   	"<!DOCTYPE svg PUBLIC '-//W3C//DTD SVG 1.1//EN' "
		"'http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd'>");
	printf( "<svg width='%d' height='%d' version='1.1' "
		"xmlns='http://www.w3.org/2000/svg' "
		"xmlns:xlink='http://www.w3.org/1999/xlink' >",
		graph_width, height);
	svg_CSS_stylesheet();
}


/* Draws a scale bar below the tree. Uses a heuristic to manage horizontal
 * space */

void draw_scale_bar(double hpos, double vpos,
		double h_scale, double d_max, char *branch_length_unit)
{
	/* Finds the largest power of 10 that is smaller than d_max (which
	 * should be the tree's depth).  Then draws as many multiples of this
	 * length as possible. If no more than one can be drawn (because it's
	 * longer than half the tree's depth), then we draw tick marks inside
	 * it instead. */

	const int big_tick_height = 7; 			/* px */
	const int units_text_voffset = -20;		/* px */
	const double interval = tick_interval(d_max);	/* user units */
	const int lbl_vspace = 2;			/* px */
	const int lbl_hspace = 2;			/* px */
	printf ("<g transform='translate(%g,%g)' style='stroke:black;stroke-width:1' >", hpos, vpos); 
	printf ("<path d='M 0 0 h %g'/>", h_scale * d_max);  

	if (scalebar_zero_at_root) {
		double x = 0;
		while (x <= d_max) {
			printf ("<path d='M %g 0 v -%d'/>", h_scale * x,
				big_tick_height);
			printf ("<text style='stroke:none;text-anchor:end'"
				" x='%g' y='-%d'>%g</text>",
				h_scale * x + lbl_hspace,
				(big_tick_height + lbl_vspace), x);
			x += interval;
		}
	} else {
		/* Time units: zero is at max depth */
		double x = d_max;
		while (x >= 0) {
			printf ("<path d='M %g 0 v -%d'/>", h_scale * x,
				big_tick_height);
			printf ("<text style='stroke:none;text-anchor:end'"
				" x='%g' y='-%d'>%g</text>",
				h_scale * x + lbl_hspace,
				(big_tick_height + lbl_vspace), d_max - x);
			x -= interval;
		}
	}


	/* Print scalebar units */
	printf ("<text style='font-size:small;stroke:none' x='0' y='%d'>"
		"%s</text>", units_text_voffset, branch_length_unit);
	printf ("</g>");
}

enum display_status display_svg_tree(
		struct rooted_tree *tree,
		enum graph_style style,
		bool align_leaves,
		bool with_scale_bar,
		char *branch_length_unit)
{	
	assert(init_done);

	/* set node positions - these are a property of the tree, and are
	 * independent of the graphics port or style */
 	if (! svg_alloc_node_pos(tree)) return DISPLAY_MEM_ERROR;
	set_node_vpos_cb(tree,
			svg_set_node_top, svg_set_node_bottom,
			svg_get_node_top, svg_get_node_bottom);
	struct h_data hd = set_node_depth_cb(tree,
			svg_set_node_depth, svg_get_node_depth);
	if (FAILURE == hd.status) return DISPLAY_MEM_ERROR;

	if (css_map)
		if (! set_group_numbers(tree))
			return DISPLAY_MEM_ERROR;

	if (ornament_map)
		if (! set_ornaments(tree))
			return DISPLAY_MEM_ERROR;

 	prettify_labels(tree);

	if (SVG_ORTHOGONAL == style)
		display_svg_tree_orthogonal(tree, hd, align_leaves,
				with_scale_bar, branch_length_unit);
	else if (SVG_RADIAL == style)
		display_svg_tree_radial(tree, hd, align_leaves,
				with_scale_bar, branch_length_unit);
	else
		return DISPLAY_UNKNOWN_STYLE;

	return DISPLAY_OK;
}

void svg_footer() { printf ("</svg>\n"); }

void destroy_svg_node_data (void *node_data)
{
	struct svg_data *data = (struct svg_data*) node_data;
	if (NULL != data) {
		if (NULL != data->ornament)
			free(data->ornament);
		free(data);
	}
}
