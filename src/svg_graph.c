/* text_graph.c - functions for drawing trees on a text canvas. */

#include <math.h>
#include <stdlib.h>
#include <stdio.h>

#include "tree.h"
#include "list.h"
#include "rnode.h"
#include "node_pos.h"
#include "redge.h"
#include "assert.h"
#include "readline.h"
#include "lca.h"
#include "hash.h"

struct colormap_pair {
	char *color;		/* a valid SVG color string, e.g. 'blue' */
	struct llist *labels;	/* color whole clade defined by those labels */
};

const int ROOT_SPACE = 10;	/* pixels */
const int LBL_SPACE = 10;	/* pixels */
const int edge_length_v_offset = -4; /* pixels */

int init_done = 0;

/* We can't pass all the parameters to write_nodes_to_g() or any other function
 * - there are too many of them - so we use external variables. */

static char *leaf_label_font_size = "medium";
static char *inner_label_font_size = "small";
static int graph_width = 300;
static char *colormap_fname = NULL;

static struct llist *colormap = NULL;

/* works OK with scale 1 on Ubuntu */
// static int char_width = 1;	/* for estimating string lengths */

/* These are setters for the external variables. This way I can keep them
 * static. I just don't like variables open to anyone, maybe I did too much
 * OO... */

void set_svg_leaf_label_font_size(char *size) { leaf_label_font_size = size; }
void set_svg_inner_label_font_size(char *size) { inner_label_font_size = size; }
void set_svg_width(int width) { graph_width = width; }
void set_svg_colormap_file(char *fname) { colormap_fname = fname; }
// void set_char_width(int width) { char_width = width; }

void svg_header()
{
	printf( "<?xml version='1.0' standalone='no'?>"
	   	"<!DOCTYPE svg PUBLIC '-//W3C//DTD SVG 1.1//EN' "
		"'http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd'>");
	printf( "<svg width='100%%' height='100%%' version='1.1' "
		"xmlns='http://www.w3.org/2000/svg'>");
}

/* Builds a colormap structure. This maps SVG color strings to lists of labels.
 * The clade detfined by the labels will be of the specified colors. This
 * structure's scope is the whole program, therefore it is an external
 * variable. */

struct llist *read_colormap()
{
	if (NULL == colormap_fname) { return NULL; }

	FILE *cmap_file = fopen(colormap_fname, "r");
	if (NULL == cmap_file) { perror(NULL); exit(EXIT_FAILURE); }

	struct llist *colormap = create_llist();

	char *line;
	while ((line = read_line(cmap_file)) != NULL) {
		/* split line into whitespace-separeted "words" */
		struct colormap_pair *cpair = malloc(sizeof(struct colormap_pair));
		if (NULL == cpair) { perror(NULL); exit(EXIT_FAILURE); }

		struct llist *label_list = create_llist();
		struct word_tokenizer *wtok = create_word_tokenizer(line);
		char *color = wt_next(wtok);
		char *label;
		while ((label = wt_next(wtok)) != NULL) {
			append_element(label_list, label);
		}
		destroy_word_tokenizer(wtok);
		free(line);
		cpair->color = color;
		cpair->labels = label_list;
		append_element(colormap, cpair);
	}

	fclose(cmap_file);

	return colormap;
}

/* A debugging function - dumps the colormap on stdout */

void dump_colormap(struct llist *colormap)
{
	printf ("Dump of colormap at %p:\n", colormap);

	struct list_elem *elem;
	for (elem = colormap->head; NULL != elem; elem = elem->next) {
		struct colormap_pair *cpair;
	       	cpair = (struct colormap_pair *) elem->data;
		struct list_elem *el;
		for (el = cpair->labels->head; NULL!=el; el=el->next) {
			printf("%s ", (char *) el->data);
		}
		printf ("-> %s\n", cpair->color);
	}
}

void svg_init()
{
	colormap = read_colormap();
	init_done = 1;
}

/* Prints the nodes to stdout, as SVG, in a <g> element. Assumes that the edges
 * have been attributed a double value in field 'length' (in this case, it is
 * done in set_node_depth()). */

void write_nodes_to_g (struct rooted_tree *tree, const double h_scale,
		const double v_scale, struct hash *node_colors)
{
	printf( "<g"
	       	" style='stroke:black;stroke-width:1;"
	    	"font-size:medium;font-weight:normal;font-family:sans'"
		" transform='translate(0,20)'"
		">");

	struct list_elem *elem;
	for (elem=tree->nodes_in_order->head; NULL!=elem; elem=elem->next) {
		struct rnode *node = (struct rnode *) elem->data;
		char *node_key = make_hash_key(node);
		struct node_pos *pos = (struct node_pos *) node->data;

		char *font_size = is_leaf(node) ?
			leaf_label_font_size : inner_label_font_size ;
		char *color;
		if ((color = hash_get(node_colors, node_key)) == NULL) {
			color = "black";
		}
		double svg_h_pos = ROOT_SPACE + (h_scale * pos->depth);
		double svg_top_pos = v_scale * pos->top; 
		double svg_bottom_pos = v_scale * pos->bottom; 
		double svg_mid_pos =
			0.5 * v_scale * (pos->top+pos->bottom);
		double svg_parent_edge_length =
			h_scale * node->parent_edge->length;

		/* draw node (vertical line) */
		printf("<line style='stroke:%s' "
			"x1='%.4f' y1='%.4f' x2='%.4f' y2='%.4f'/>",
			color, svg_h_pos, svg_top_pos, svg_h_pos,
			svg_bottom_pos);
		/* draw label */
		printf("<text style='stroke:none;font-size:%s' "
		       "x='%.4f' y='%.4f'>%s</text>",
			font_size, svg_h_pos + LBL_SPACE,
			svg_mid_pos, node->label);
		/* draw horizontal line */
		if (is_root(node)) {
			printf("<line x1='0' y1='%.4f' x2='%.4f' y2='%.4f'/>",
				svg_mid_pos, svg_h_pos, svg_mid_pos);

		} else {
			printf ("<line style='stroke:%s' "
				"x1='%.4f' y1='%.4f' x2='%.4f' y2='%.4f'/>",
				color, svg_h_pos - svg_parent_edge_length,
				 svg_mid_pos, svg_h_pos, svg_mid_pos);
			printf ("<text style='stroke:none;font-size:%s' "
				"x='%4f' y='%4f'>%s</text>",
				inner_label_font_size,
				svg_h_pos - 0.5 * svg_parent_edge_length,
				edge_length_v_offset + svg_mid_pos,
				node->parent_edge->length_as_string);

		}

	}
	printf("</g>");
}

/* Passed to dump_llist() for labels */
void dump_label (void *lbl) { puts((char *) lbl); }

/* Constructs a hash of node colors from the colormap. Key is node's address,
 * value is a color string. Nodes are specified by their labels or descendant
 * labels ('labels' member of a colormap_pair structure) NOTE: contrary to the
 * colormap itself, this structure is dependent on the tree it is passed to,
 * since the LCA of a list of labels depends on the tree. So it has to be
 * computed for each tree. */

struct hash *set_node_colors(struct rooted_tree *tree)
{
	struct hash *node_colors = create_hash(tree->nodes_in_order->count);

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
		char *lca_key = make_hash_key(lca);
		hash_set(node_colors, lca_key, cpair->color);	
		// printf ("%s -> %s\n", lca->label, cpair->color);
	}

	/* Attribute colors to other nodes according to ancestors */
	struct llist *nodes_in_reverse_order;
	nodes_in_reverse_order = llist_reverse(tree->nodes_in_order);
	struct list_elem *el;
	for (el=nodes_in_reverse_order->head; NULL!=el; el=el->next) {
		struct rnode *node = (struct rnode *) el->data;
		char *hash_key = make_hash_key(node);
		if (NULL == hash_get(node_colors, hash_key)) {
			if (is_root(node)) {
				hash_set(node_colors, hash_key, "black");
			} else {
				struct rnode *parent;
				parent = node->parent_edge->parent_node;
				char *parent_hash_key;
				parent_hash_key = make_hash_key(parent);
				char *color = (char *) hash_get(node_colors,
						parent_hash_key);
				assert(NULL != color);
				free(parent_hash_key);
				hash_set(node_colors, hash_key, color);
			}
		}
		/* if not NULL, just leave as is */
		free(hash_key);
	}
	destroy_llist(nodes_in_reverse_order);

	return node_colors;
}

void display_svg_tree(struct rooted_tree *tree)
{	
	/* Ensure that init has been done */
	assert(0 != init_done);

	/* set node positions */
	alloc_node_pos(tree);
	int num_leaves = set_node_vpos(tree);
	struct h_data hd = set_node_depth(tree);
	double h_scale = -1;
	double v_scale = 40.0; // TODO: set as parameter

	struct hash *node_colors = set_node_colors(tree);


	if (0.0 == hd.d_max ) { hd.d_max = 1; } 	/* one-node trees */
	/* create canvas and draw nodes on it */
	h_scale = (graph_width - hd.l_max - ROOT_SPACE - LBL_SPACE) / hd.d_max;
	write_nodes_to_g(tree, h_scale, v_scale, node_colors);
}

void svg_footer() { printf ("</svg>"); }
