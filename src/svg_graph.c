/* text_graph.c - functions for drawing trees on a text canvas. */

#define _GNU_SOURCE

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "tree.h"
#include "list.h"
#include "rnode.h"
#include "redge.h"
#include "assert.h"
#include "readline.h"
#include "lca.h"
#include "hash.h"
#include "svg_graph.h"
#include "node_pos_alloc.h"
#include "common.h"
#include "xml_utils.h"
#include "graph_common.h"
#include "svg_graph_common.h"
#include "svg_graph_radial.h"
#include "svg_graph_ortho.h"

struct css_map_element {
	int clade_nb;	
	char *style;	/* a valid CSS specification */
	struct llist *labels;
};


char *leaf_label_class = "leaf-label";
char *inner_label_class = "inner-label";

int init_done = FALSE;

/* We can't pass all the parameters to write_nodes_to_g() or any other function
 * - there are too many of them - so we use external variables. */

int graph_width = -1;
static char *colormap_fname = NULL;
int svg_whole_v_shift = -1; /* Vertical translation of whole graph */
static int graph_style = -1;
static FILE *url_map_file = NULL;
static FILE *css_map_file = NULL;
static char *leaf_label_style = NULL;
static char *inner_label_style = NULL;
static char *edge_label_style = NULL;

static struct llist *css_map = NULL;
struct hash *url_map = NULL;

/* These are setters for the external variables. This way I can keep them
 * static. I just don't like variables open to anyone, maybe I did too much
 * OO... */

void set_svg_width(int width) { graph_width = width; }
void set_svg_colormap_file(char *fname) { colormap_fname = fname; }
void set_svg_whole_v_shift(int shift) { svg_whole_v_shift = shift; }
void set_svg_style(int style) { graph_style = style; }
void set_svg_URL_map_file(FILE * map) { url_map_file = map; }
void set_svg_CSS_map_file(FILE * map) { css_map_file = map; }
void set_svg_leaf_label_style(char *style) { leaf_label_style = style; }
void set_svg_inner_label_style(char *style) { inner_label_style = style; }
void set_svg_edge_label_style(char *style) { edge_label_style = style; }

void svg_CSS_stylesheet()
{
	struct list_elem *el;

	printf ("<defs><style type='text/css'><![CDATA[\n");
	if (css_map) {
		for (el = css_map->head; NULL != el; el = el->next) {
			struct css_map_element *css_el = el->data;
			printf(" .clade_%d {%s}\n", css_el->clade_nb,
					css_el->style);
		}
	}
	printf (" .leaf-label {%s}\n", leaf_label_style);
	printf (" .inner-label {%s}\n", inner_label_style);
	printf (" .edge-label {%s}\n", edge_label_style);
	printf ("]]></style></defs>");
}

void svg_header()
{
	printf( "<?xml version='1.0' standalone='no'?>"
	   	"<!DOCTYPE svg PUBLIC '-//W3C//DTD SVG 1.1//EN' "
		"'http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd'>");
	printf( "<svg width='100%%' height='100%%' version='1.1' "
		"xmlns='http://www.w3.org/2000/svg' "
		"xmlns:xlink='http://www.w3.org/1999/xlink' >");
	svg_CSS_stylesheet();
}

/* Builds a colormap structure. This maps SVG color strings to lists of labels.
 * The clade defined by the labels will be of the specified colors. This
 * structure's scope is the whole program, therefore it is an external
 * variable. */

/* Builds a CSS map structure. This is a list of css_map_element elements, each
 * of which contains a style specification and a list of labels. The style will
 * apply to the clade defined by the labels. */

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

		/* split line into whitespace-separeted "words" */
		struct llist *label_list = create_llist();
		struct word_tokenizer *wtok = create_word_tokenizer(line);
		char *style = wt_next(wtok);
		char *label;
		while ((label = wt_next(wtok)) != NULL) {
			append_element(label_list, label);
		}
		destroy_word_tokenizer(wtok);
		free(line);
		css_el->style = style;
		css_el->labels = label_list;
		css_el->clade_nb = i;
		append_element(css_map, css_el);
		i++;
	}

	fclose(css_map_file);

	return css_map;
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
		// TODO: maybe we can do without asprintf()
		asprintf(&anchor_attributes, "xlink:href='%s' ", escaped_url);
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
	//colormap = read_colormap();
	css_map = read_css_map();
	url_map = read_url_map();
	init_done = 1;
}

/* Passed to dump_llist() for labels */
void dump_label (void *lbl) { puts((char *) lbl); }

#if 0
/* Constructs a hash of node colors from the colormap. Key is node's address,
 * value is a color string. Nodes are specified by their labels or descendant
 * labels ('labels' member of a colormap_pair structure) NOTE: contrary to the
 * colormap itself, this structure is dependent on the tree it is passed to,
 * since the LCA of a list of labels depends on the tree. So it has to be
 * computed for each tree. */

void set_node_colors(struct rooted_tree *tree)
{
	/* Attribute colors to LCA of labels as specified in the color map */
	struct list_elem *elem;
	for (elem=colormap->head; NULL!=elem; elem=elem->next) {
		struct colormap_pair *cpair;
	       	cpair = (struct colormap_pair *) elem->data;
		struct llist *labels = cpair->labels;
		/* printf ("Labels:\n");
		dump_llist(labels, dump_label);
		printf ("Color: %s\n", cpair->color); */
		struct rnode *lca = lca_from_labels(tree, labels);
		((struct svg_data *) lca->data)->color = cpair->color;
		// printf ("%s -> %s\n", lca->label, cpair->color);
	}

	/* Attribute colors to other nodes according to ancestors */
	struct llist *nodes_in_reverse_order;
	nodes_in_reverse_order = llist_reverse(tree->nodes_in_order);
	struct list_elem *el;
	for (el=nodes_in_reverse_order->head; NULL!=el; el=el->next) {
		struct rnode *node = (struct rnode *) el->data;
		struct svg_data *node_data = (struct svg_data *) node->data;
		if (NULL == node_data->color) {
			if (is_root(node)) {
				node_data->color = "black";
			} else {
				struct rnode *parent;
				parent = node->parent_edge->parent_node;
				char *parent_color =
					((struct svg_data *) parent->data)
					->color;
				assert(NULL != parent_color);
				node_data->color = parent_color;
			}
		}
	}
	destroy_llist(nodes_in_reverse_order);
}
#endif


/* Attributes numbers to clades, based on the CSS style map (if one was
 * supplied - see read_css_map() and svg_CSS_stylesheet() ). The clade number
 * will translate directly into 'class' attributes in SVG, which in turn will
 * have a style defined according to the style map. */

void set_clade_numbers(struct rooted_tree *tree)
{
	/* Iterate through the style map elements. Each one contains (among
	 * others) a list of labels. Find the LCA of those labels, and set its
	 * style to that specified by the map. */
	struct list_elem *elem;
	for (elem = css_map->head; NULL != elem; elem = elem->next) {
		struct css_map_element *css_el = elem->data;
		struct llist *labels = css_el->labels;
		struct rnode *lca = lca_from_labels(tree, labels);
		struct svg_data *lca_data = lca->data;
		lca_data->clade_nb = css_el->clade_nb;
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
		if (UNSTYLED_CLADE == node_data->clade_nb) {
			node_data->clade_nb = parent_data->clade_nb;
		}
				
	}
	destroy_llist(nodes_in_reverse_order);
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
		svgd->clade_nb = UNSTYLED_CLADE;	
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

	if (css_map) set_clade_numbers(tree);
 	prettify_labels(tree);

	if (SVG_ORTHOGONAL == graph_style)
		display_svg_tree_orthogonal(tree, hd, align_leaves,
				with_scale_bar, branch_length_unit);
	else if (SVG_RADIAL == graph_style)
		display_svg_tree_radial(tree, hd, align_leaves);
	else
		// TODO: better handling of errors: should return error code
		fprintf (stderr, "Unknown tree style %d\n", graph_style);
		
}

void svg_footer() { printf ("</svg>"); }
