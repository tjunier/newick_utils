/* text_graph.c - functions for drawing trees on a text canvas. */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <ctype.h>

#include "common.h"
#include "graph_common.h"
#include "hash.h"
#include "lca.h"
#include "list.h"
#include "masprintf.h"
#include "node_pos_alloc.h"
#include "nodemap.h"
#include "readline.h"
#include "redge.h"
#include "rnode.h"
#include "svg_graph_common.h"
#include "svg_graph.h"
#include "svg_graph_ortho.h"
#include "svg_graph_radial.h"
#include "tree.h"
#include "xml_utils.h"

enum { INDIVIDUAL, CLADE, UNKNOWN };

struct css_map_element {
	int group_type;	/* INDIVIDUAL or CLADE */
	int group_nb;	
	char *style;	/* a CSS specification */
	struct llist *labels;
};

struct ornament_map_element {
	int group_type;	/* INDIVIDUAL or CLADE */
	char *ornament;	/* an SVG snippet */
	struct llist *labels;
};

// TODO: #define as constants in svg_graph_common.h ?
char *leaf_label_class = "leaf-label";
char *inner_label_class = "inner-label";

int init_done = FALSE;

/************************** external variables *****************************/

/* We can't pass all the parameters to display_svg_tree() or any other function
 * - there are too many of them - so we use external variables. The static ones
 *   are only used here, the other ones have external linkage because they are
 *   used in other modules. */

static int graph_style = -1;
static FILE *url_map_file = NULL;
static FILE *css_map_file = NULL;
static FILE *ornament_map_file = NULL;
static char *leaf_label_style = NULL;
static char *inner_label_style = NULL;
static char *edge_label_style = NULL;
static char *plain_node_style = NULL;
static struct llist *css_map = NULL;
static struct llist *ornament_map = NULL;

struct hash *url_map = NULL;
int graph_width = -1;
int svg_whole_v_shift = -1; 	/* Vertical translation of whole graph */
double label_char_width = -1;

/* These are setters for the external variables. This way I can keep most of
 * them static. I just don't like variables open to anyone, maybe I did too
 * much OO... I also provide setters for the nonstatic ones, so the interface
 * is more homogeneous. */

void set_svg_width(int width) { graph_width = width; }
void set_svg_whole_v_shift(int shift) { svg_whole_v_shift = shift; }
void set_svg_style(int style) { graph_style = style; }
void set_svg_URL_map_file(FILE * map) { url_map_file = map; }
void set_svg_CSS_map_file(FILE * map) { css_map_file = map; }
void set_svg_ornament_map_file(FILE * map) { ornament_map_file = map; }
void set_svg_leaf_label_style(char *style) { leaf_label_style = style; }
void set_svg_inner_label_style(char *style) { inner_label_style = style; }
void set_svg_edge_label_style(char *style) { edge_label_style = style; }
void set_svg_plain_node_style(char *style) { plain_node_style = style; }
void set_svg_label_char_width(double width) { label_char_width = width; }

/************************** functions *****************************/

void svg_CSS_stylesheet()
{
	struct list_elem *el;

	printf ("<defs><style type='text/css'><![CDATA[\n");
	/* The default clade style is specified in this file rather than in the
	 * client code (TODO: try to get all defaults in the same place, or
	 * explain why not), so we print the style only if not NULL */
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

/* Prints the SVG header. The first argument is the tree's number of leaves,
 * needed to compute height for orthogonal trees; the second argument is a
 * boolean that is true IFF we show a scale bar. */

void svg_header(int nb_leaves, int with_scale_bar)
{
	int height;

	switch (graph_style) {
		case SVG_ORTHOGONAL:
			/* image fits in a rectangle, so height != width */
			/* we add 1 leaf skip for the scale bar if needed */
			if (with_scale_bar) 
				++nb_leaves;
			height = nb_leaves * leaf_vskip;
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

struct llist *read_css_map()
{
	if (NULL == css_map_file)  return NULL; 

	struct llist *css_map = create_llist();

	char *line;
	int i = 1;
	while ((line = read_line(css_map_file)) != NULL) {
		struct css_map_element *css_el = malloc(
				sizeof(struct css_map_element));
		if (NULL == css_el) { perror(NULL); exit(EXIT_FAILURE); }

		/* Split line into whitespace-separated "words" (or words
		 * delimited by double quotes). First word is the CSS
		 * specification, second is group type, then come the labels.
		 * */
		struct llist *label_list = create_llist();
		struct word_tokenizer *wtok = create_word_tokenizer(line);
		char *style = wt_next(wtok);
		char *type = wt_next(wtok);
		char *label;
		while ((label = wt_next(wtok)) != NULL) {
			append_element(label_list, label);
		}
		destroy_word_tokenizer(wtok);
		free(line);
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
		append_element(css_map, css_el);
		i++;

		free(type);
	}

	fclose(css_map_file);

	return css_map;
}

/* Builds an ornament map structure. This is like the CSS map, but for
 * ornaments. */

struct llist *read_ornament_map()
{
	if (NULL == ornament_map_file)  return NULL; 

	struct llist *ornament_map = create_llist();

	char *line;
	while ((line = read_line(ornament_map_file)) != NULL) {
		struct ornament_map_element *oel = malloc(
				sizeof(struct ornament_map_element));
		if (NULL == oel) { perror(NULL); exit(EXIT_FAILURE); }

		/* Split line into whitespace-separated "words" (or words
		 * delimited by double quotes). First word is the ornament ,
		 * second is group type, then come the labels.  */

		struct llist *label_list = create_llist();
		struct word_tokenizer *wtok = create_word_tokenizer(line);
		char *ornament = wt_next_noquote(wtok);
		char *type = wt_next(wtok);
		char *label;
		while ((label = wt_next(wtok)) != NULL) {
			append_element(label_list, label);
		}
		destroy_word_tokenizer(wtok);
		free(line);
		oel->group_type = get_group_type(type); 
		if (UNKNOWN == oel->group_type) {
			fprintf (stderr, "WARNING: unknown group type '%s' (ignored)\n", type);
			free(oel);
			free(type);
			destroy_llist(label_list);
			continue;
		}
		oel->ornament = ornament;
		oel->labels = label_list;
		append_element(ornament_map, oel);

		free(type);
	}

	fclose(ornament_map_file);

	return ornament_map;
}

/* Reads in the URL map (label -> URL) */

struct hash *read_url_map()
{
	if (NULL == url_map_file) return NULL;

	struct hash *url_map = create_hash(URL_MAP_SIZE);

	char *line;
	while ((line = read_line(url_map_file)) != NULL) {
		struct word_tokenizer *wtok = create_word_tokenizer(line);
		char *label = wt_next(wtok);
		underscores2spaces(label);
		remove_quotes(label);
		char *url = wt_next(wtok);
		char *escaped_url = escape_predefined_character_entities(url);
		char *anchor_attributes;
		anchor_attributes = masprintf("xlink:href='%s' ", escaped_url);
		if (NULL == anchor_attributes) {
			perror(NULL); exit (EXIT_FAILURE);
		}
		char *att;
		while ((att = wt_next(wtok)) != NULL) {
			int att_len = strlen(anchor_attributes);
			/* add length of new attribute */
			att_len += strlen(att);
			att_len += 1;	/* trailing space */
			att_len += 1;	/* terminal '\0' */
			anchor_attributes = realloc(anchor_attributes,
				att_len * sizeof(char));
			if (NULL == anchor_attributes) {
				perror(NULL);
				exit(EXIT_FAILURE);
			}
			strcat(anchor_attributes, att);
			strcat(anchor_attributes, " ");

		}
		hash_set(url_map, label, anchor_attributes);
		destroy_word_tokenizer(wtok);
		free(line);
		free(label);
		free(url);
		free(escaped_url);
	}

	return url_map;
}

/* Call this function before calling display_svg_tree(). Resist the temptation
 * to put it inside display_svg_tree(): it is kept separate because its job is
 * not directly to draw trees*/

void svg_init()
{
	css_map = read_css_map();
	ornament_map = read_ornament_map();
	url_map = read_url_map();
	init_done = 1;
}

/* Passed to dump_llist() for labels */
void dump_label (void *lbl) { puts((char *) lbl); }

/* Attributes group numbers to nodes, based on the CSS style map (if one was
 * supplied - see read_css_map() and svg_CSS_stylesheet() ). The group number
 * will translate directly into 'class' attributes in SVG, which in turn will
 * have a style defined according to the style map. */

// NOTE: this could be made more efficient by having two separate maps, one for
// CLADE elements and another for INDIVIDUAL elements. That way the list needs
// not be traversed twice. But all in all it will likely not make a big
// difference, so I'll keep it for later :-) 

void set_group_numbers(struct rooted_tree *tree)
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
		struct svg_data *lca_data = lca->data;
		lca_data->group_nb = css_el->group_nb;
	}

	/* Now propagate the styles to the descendants */
	struct llist *nodes_in_reverse_order;
	nodes_in_reverse_order = llist_reverse(tree->nodes_in_order);
	struct list_elem *el; /* TODO: can't I reuse elem from above? */
	el = nodes_in_reverse_order->head->next;	/* skip root */
	for (;  NULL != el; el = el->next) {
		struct rnode *node = el->data;
		struct svg_data *node_data = node->data;
		struct rnode *parent = node->parent_edge->parent_node;
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
	// TODO: think of making the label2node map a member of the tree structure.
	struct hash *map = create_label2node_list_map(tree->nodes_in_order);
	for (elem = css_map->head; NULL != elem; elem = elem->next) {
		css_el = elem->data;
		if (INDIVIDUAL != css_el->group_type) continue;
		struct llist *labels = css_el->labels;
		struct llist *group_nodes = create_llist();
		/* Iterate over all labels of this element, adding the
		 * corresponding nodes to 'group_nodes' */
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
}

/* Attributes ornaments to nodes, based on the ornament map (if one was
 * supplied - see read_ornament_map()). The ornament will be printed directly
 * at the node's position. */

// NOTE: this could be made more efficient by having two separate maps, one for
// CLADE elements and another for INDIVIDUAL elements. That way the list needs
// not be traversed twice. But all in all it will likely not make a big
// difference, so I'll keep it for later :-) 

void set_ornaments(struct rooted_tree *tree)
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
		struct svg_data *lca_data = lca->data;
		lca_data->ornament = strdup(oel->ornament);
	}

	/* Now iterate through the INDIVIDUAL style map elements. They also
	 * contain a list of labels. Each label is matched by at least 1 node.
	 * All of these nodes get the ornament. */

	// TODO: think of making the label2node map a member of the tree structure.
	struct hash *map = create_label2node_list_map(tree->nodes_in_order);
	for (elem = ornament_map->head; NULL != elem; elem = elem->next) {
		oel = elem->data;
		if (INDIVIDUAL != oel->group_type) continue;
		struct llist *labels = oel->labels;
		struct llist *group_nodes = create_llist();
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
}

/* Allocates and initializes an svg_data structure for each of the tree's
 * nodes. The real data are set later through callbacks (see
 * svg_set_node_top(), etc) */

void svg_alloc_node_pos(struct rooted_tree *tree) 
{
	struct list_elem *le;
	struct rnode *node;

	for (le = tree->nodes_in_order->head; NULL != le; le = le->next) {
		node = le->data;
		struct svg_data *svgd = malloc(sizeof(struct svg_data));
		if (NULL == svgd) { perror(NULL); exit (EXIT_FAILURE); }
		svgd->top = svgd->bottom = svgd->depth = -1.0;
		svgd->group_nb = UNSTYLED_CLADE;	
		svgd->ornament = NULL;
		node->data = svgd;
	}
}

void svg_set_node_top (struct rnode *node, double top)
{
	((struct svg_data *) node->data)->top = top;
}

void svg_set_node_bottom (struct rnode *node, double bottom)
{
	((struct svg_data *) node->data)->bottom = bottom;
}

double svg_get_node_top (struct rnode *node)
{
	return ((struct svg_data *) node->data)->top;
}

double svg_get_node_bottom (struct rnode *node)
{
	return ((struct svg_data *) node->data)->bottom;
}

void svg_set_node_depth (struct rnode *node, double depth)
{
	((struct svg_data *) node->data)->depth = depth;
}

double svg_get_node_depth (struct rnode *node)
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

/* Draws a scale bar below the tree. Uses a heuristic to manage horizontal
 * space */

// TODO: first two args should both be double
//
void draw_scale_bar(int hpos, double vpos,
		double h_scale, double d_max, char *branch_length_unit)
{
	/* Finds the largest power of 10 that is smaller than d_max (which
	 * should be the tree's depth).  Then draws as many multiples of this
	 * length as possible. If no more than one can be drawn (because it's
	 * longer than half the tree's depth), then we draw tick marks inside
	 * it instead. */

	const int big_tick_height = 5; 			/* px */
	const int small_tick_height = 3;		/* px */
	const int units_text_voffset = -15;		/* px */
	const int vsep = 1;				/* px */
	double pot = largest_PoT_lte(d_max);		/* tree units */
	double scale_length = pot * h_scale;		/* px */

	printf ("<g transform='translate(%d,%g)'>", hpos, vpos); 

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
		/* Print as many multiples of 'scale_length' as  will fit */

		/* Find how many multiples fit */
		int multiples;
		for (multiples = 0; (multiples+1)*pot < d_max; multiples++)
			;
		
		/* Print base line and tick marks */
		printf ("<g style='stroke:black;stroke-linecap:round'>");
		printf ("<path d='M 0 0 h %g'/>",
				big_tick_height, multiples * scale_length);
		int i;
		for (i = 0; i <= multiples; i++) {
			printf ("<path style='stroke:black' d='M %g 0 v %d'/>",
				i*scale_length, -big_tick_height);
		}
		printf ("</g>");

		/* Print tick labels */
		printf ("<g style='font-size:small;stroke:none;text-anchor:end'>");
		for (i = 0; i <= multiples; i++) { 
			printf ("<text x='%g' y='%d'>%g</text>",
				i*scale_length, -big_tick_height, i*pot);
		}
		printf ("</g>");
	}
	printf ("<text style='font-size:small;stroke:none' x='0' y='%d'>"
		"%s</text>", units_text_voffset, branch_length_unit);
	printf ("</g>");
}

void display_svg_tree(struct rooted_tree *tree, int align_leaves,
		int with_scale_bar, char *branch_length_unit)
{	
	assert(init_done);

	/* set node positions - these are a property of the tree, and are
	 * independent of the graphics port or style */
 	svg_alloc_node_pos(tree);
	set_node_vpos_cb(tree,
			svg_set_node_top, svg_set_node_bottom,
			svg_get_node_top, svg_get_node_bottom);
	struct h_data hd = set_node_depth_cb(tree,
			svg_set_node_depth, svg_get_node_depth);

	if (css_map) set_group_numbers(tree);
	if (ornament_map) set_ornaments(tree);
 	prettify_labels(tree);

	if (SVG_ORTHOGONAL == graph_style)
		display_svg_tree_orthogonal(tree, hd, align_leaves,
				with_scale_bar, branch_length_unit);
	else if (SVG_RADIAL == graph_style)
		display_svg_tree_radial(tree, hd, align_leaves,
				with_scale_bar, branch_length_unit);
	else
		// TODO: better handling of errors: should return error code
		fprintf (stderr, "Unknown tree style %d\n", graph_style);
}

void svg_footer() { printf ("</svg>"); }
