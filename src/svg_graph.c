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
	dump_colormap(colormap);
	init_done = 1;
}

/* Prints the nodes to stdout, as SVG, in a <g> element. Assumes that the edges
 * have been attributed a double value in field 'length' (in this case, it is
 * done in set_node_depth()). */

void write_nodes_to_g (struct rooted_tree *tree, const double h_scale,
		const double v_scale)
{
	printf( "<g"
	       	" style='stroke:black;stroke-width:1;"
	    	"font-size:medium;font-weight:normal;font-family:sans'"
		" transform='translate(0,20)'"
		">");

	struct list_elem *elem;

	for (elem = tree->nodes_in_order->head; NULL != elem; elem = elem->next) {
		struct rnode *node = (struct rnode *) elem->data;
		struct node_pos *pos = (struct node_pos *) node->data;
		char *font_size;
		/* draw node (vertical line) */
		printf("<line x1='%.4f' y1='%.4f' x2='%.4f' y2='%.4f'/>",
				rint(ROOT_SPACE + (h_scale * pos->depth)),
				rint(2 * v_scale * pos->top),
				rint(ROOT_SPACE + (h_scale * pos->depth)),
				rint(2 * v_scale * pos->bottom)
			);
		/* draw label */
		font_size = is_leaf(node) ? leaf_label_font_size : inner_label_font_size ;
		printf("<text style='stroke:none;font-size:%s' x='%.4f' y='%.4f'>%s</text>",
				font_size,
				rint(ROOT_SPACE + (h_scale * pos->depth) + LBL_SPACE), 
				rint(v_scale * (pos->top+pos->bottom)),
				node->label
				);
		/* draw horizontal line */
		if (is_root(node)) {
			printf("<line x1='0' y1='%.4f' x2='%.4f' y2='%.4f'/>",
					rint(v_scale * (pos->top+pos->bottom)),
					rint(ROOT_SPACE + (h_scale * pos->depth)),
					rint(v_scale * (pos->top+pos->bottom)));

		} else {
			printf ("<line x1='%.4f' y1='%.4f' x2='%.4f' y2='%.4f'/>",
				 rint(ROOT_SPACE + h_scale * (pos->depth - node->parent_edge->length)),
				 rint(v_scale * (pos->top + pos->bottom)), /* (2*top + 2*bottom) / 2 */
				 rint(ROOT_SPACE + h_scale * (pos->depth)),
				 rint(v_scale * (pos->top + pos->bottom)));
			printf ("<text style='stroke:none;font-size:%s' x='%4f' y='%4f'>%s</text>",
					inner_label_font_size,
					rint(ROOT_SPACE + h_scale * (pos->depth - 0.5 * node->parent_edge->length)),
					rint(edge_length_v_offset + v_scale * (pos->top + pos->bottom)),
					node->parent_edge->length_as_string);

		}

	}
	printf("</g>");
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
	double v_scale = 20.0;

	if (0.0 == hd.d_max ) { hd.d_max = 1; } 	/* one-node trees */
	/* create canvas and draw nodes on it */
	h_scale = (graph_width - hd.l_max - ROOT_SPACE - LBL_SPACE) / hd.d_max;
	write_nodes_to_g(tree, h_scale, v_scale);
}

void svg_footer() { printf ("</svg>"); }
