/* text_graph.c - functions for drawing trees on a text canvas. */

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

void underscores2spaces(); /* defined in display.c */

struct colormap_pair {
	char *color;		/* a valid SVG color string, e.g. 'blue' */
	struct llist *labels;	/* color whole clade defined by those labels */
};

/* rnode data for SVG trees */

struct svg_data {
	double top;
	double bottom;
	double depth;
	char *color;
	/* ... other node properties ... */
};

const int ROOT_SPACE = 10;	/* pixels */
const int LBL_SPACE = 10;	/* pixels */
const int INNER_LBL_SPACE = 4;	/* pixels */
const int CHAR_WIDTH = 5;	/* pixels, approximattion for 'medium' fonts */
const int edge_length_v_offset = -4; /* pixels */
const double PI = 3.14159;
const int URL_MAP_SIZE = 100;	/* bins */

int init_done = FALSE;

/* We can't pass all the parameters to write_nodes_to_g() or any other function
 * - there are too many of them - so we use external variables. */

/* SVG font sizes are strings, to allow values like 'small', etc. */
static char *leaf_label_font_size = NULL;
static char *inner_label_font_size = NULL;
static char *branch_length_font_size = NULL;

static int graph_width = -1;
static char *colormap_fname = NULL;
static double leaf_vskip = -1; 	/* Vertical separation of leaves (px) */
static int svg_whole_v_shift = -1; /* Vertical translation of whole graph */
/* If this is zero, the label's baseline is aligned on the branch. Use it to
 * nudge the labels a small angle. Unfortunately the correct amount will depend
 * on the graph's diameter and the label font size. */
static double svg_label_angle_correction = 0.0;
/* The following applies to labels on the left side of the tree (because they
 * are subject to a 180° rotation, draw_text_radial() */
static double svg_left_label_angle_correction = -0.0349; /* -2°, in radians */ 
static int graph_style = -1;
static FILE *url_map_file = NULL;

static struct llist *colormap = NULL;
static struct hash *url_map = NULL;

/* These are setters for the external variables. This way I can keep them
 * static. I just don't like variables open to anyone, maybe I did too much
 * OO... */

void set_svg_leaf_label_font_size(char *size) { leaf_label_font_size = size; }
void set_svg_inner_label_font_size(char *size) { inner_label_font_size = size; }
void set_svg_branch_length_font_size(char *size) {
	branch_length_font_size = size; }
void set_svg_width(int width) { graph_width = width; }
void set_svg_colormap_file(char *fname) { colormap_fname = fname; }
void set_svg_leaf_vskip(double skip) { leaf_vskip = skip; }
void set_svg_whole_v_shift(int shift) { svg_whole_v_shift = shift; }
void set_svg_left_label_angle_correction(double corr) {
	svg_left_label_angle_correction = corr; }
void set_svg_label_angle_correction(double corr) {
	svg_label_angle_correction = corr; }
void set_svg_style(int style) { graph_style = style; }
void set_svg_URL_map_file(FILE * map) { url_map_file = map; }

void svg_header()
{
	printf( "<?xml version='1.0' standalone='no'?>"
	   	"<!DOCTYPE svg PUBLIC '-//W3C//DTD SVG 1.1//EN' "
		"'http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd'>");
	printf( "<svg width='100%%' height='100%%' version='1.1' "
		"xmlns='http://www.w3.org/2000/svg' "
		"xmlns:xlink='http://www.w3.org/1999/xlink' >");
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

/* Reads in the URL map (label -> URL) */

struct hash *read_url_map()
{
	if (NULL == url_map_file) return NULL;

	struct hash *url_map = create_hash(URL_MAP_SIZE);

	char *line;
	while ((line = read_line(url_map_file)) != NULL) {
		struct word_tokenizer *wtok = create_word_tokenizer(line);
		char *label = wt_next(wtok);
		char *url = wt_next(wtok);
		char *escaped_url = escape_predefined_character_entities(url);
		hash_set(url_map, label, escaped_url);
		destroy_word_tokenizer(wtok);
		free(line);
		free(label);
		free(url);
	}

	return url_map;
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

/* Call this function before calling display_svg_tree(). Resist the temptation
 * to put it inside display_svg_tree(): it is kept separate because its job is
 * not directly to draw trees*/

void svg_init()
{
	colormap = read_colormap();
	url_map = read_url_map();
	init_done = 1;
}

/* Outputs an SVG <g> element with all the tree branches, orthogonal */

void draw_branches_ortho (struct rooted_tree *tree, const double h_scale,
		const double v_scale, int align_leaves, double dmax)
{
	printf( "<g"
	       	" style='stroke:black;stroke-width:1;"
	    	"font-size:medium;font-weight:normal;font-family:sans'>");

	struct list_elem *elem;
	for (elem=tree->nodes_in_order->head; NULL!=elem; elem=elem->next) {
		struct rnode *node = elem->data;
		struct svg_data *node_data = (struct svg_data *) node->data;

		/* For cladograms */
		if (align_leaves && is_leaf(node))
			node_data->depth = dmax;

		char *color = node_data->color;
		if (NULL == color) color = "black";
		double svg_h_pos = ROOT_SPACE + (h_scale * node_data->depth);
		double svg_top_pos = v_scale * node_data->top; 
		double svg_bottom_pos = v_scale * node_data->bottom; 
		double svg_mid_pos =
			0.5 * v_scale * (node_data->top+node_data->bottom);

		/* draw node (vertical line), except for leaves */
		if (! is_leaf(node)) {
		printf("<line style='stroke:%s' stroke-linecap='round' "
			"x1='%.4f' y1='%.4f' x2='%.4f' y2='%.4f'/>",
			color, svg_h_pos, svg_top_pos, svg_h_pos,
			svg_bottom_pos);
		}
		/* draw horizontal line */
		if (is_root(node)) {
			printf("<line stroke-linecap='round' "
				"x1='0' y1='%.4f' x2='%.4f' y2='%.4f'/>",
				svg_mid_pos, svg_h_pos, svg_mid_pos);

		} else {
			struct rnode *parent = node->parent_edge->parent_node;
			struct svg_data *parent_data = parent->data;
			double svg_parent_h_pos = ROOT_SPACE + (
				h_scale * parent_data->depth);
			printf ("<line style='stroke:%s' "
				"stroke-linecap='round' "
				"x1='%.4f' y1='%.4f' x2='%.4f' y2='%.4f'/>",
				color, svg_parent_h_pos,
				 svg_mid_pos, svg_h_pos, svg_mid_pos);
		}

	}
	printf("</g>");
}

/* Outputs an SVG <g> element with all the tree branches, radial. In this
 * context, a node's 'top' and 'bottom' are angles, not vertical positions */

void draw_branches_radial (struct rooted_tree *tree, const double r_scale,
		const double a_scale, int align_leaves, double dmax)
{
	printf( "<g"
	       	" style='stroke:black;stroke-width:1;fill:none'>"
	    	);

	struct list_elem *elem;
	for (elem=tree->nodes_in_order->head; NULL!=elem; elem=elem->next) {
		struct rnode *node = elem->data;
		struct svg_data *node_data = (struct svg_data *) node->data;

		/* For cladograms */
		if (align_leaves && is_leaf(node))
			node_data->depth = dmax;

		char *color = node_data->color;
		if (NULL == color) color = "black";
		double svg_radius = ROOT_SPACE + (r_scale * node_data->depth);
		double svg_top_angle = a_scale * node_data->top; 
		double svg_bottom_angle = a_scale * node_data->bottom; 
		double svg_mid_angle =
			0.5 * a_scale * (node_data->top+node_data->bottom);
		double svg_mid_x_pos = svg_radius * cos(svg_mid_angle);
		double svg_mid_y_pos = svg_radius * sin(svg_mid_angle);
		int large_arc_flag;
		if (svg_bottom_angle - svg_top_angle > PI)
			large_arc_flag = 1; /* keep 1 and 0: literal SVG flags */
		else
			large_arc_flag = 0;

		/* draw node (arc), except for leaves */
		if (! is_leaf(node)) {
			double svg_top_x_pos = svg_radius * cos(svg_top_angle);
			double svg_top_y_pos = svg_radius * sin(svg_top_angle);
			double svg_bot_x_pos = svg_radius * cos(svg_bottom_angle);
			double svg_bot_y_pos = svg_radius * sin(svg_bottom_angle);
			printf("<path style='stroke:%s' stroke-linecap='round'"
			       " d='M%.4f,%.4f A%4f,%4f 0 %d 1 %.4f %.4f'/>",
				color,
				svg_top_x_pos, svg_top_y_pos,
				svg_radius, svg_radius,
				large_arc_flag,
				svg_bot_x_pos, svg_bot_y_pos);
		}
		/* draw radial line */
		if (is_root(node)) {
			printf("<line stroke-linecap='round' "
				"x1='0' y1='0' x2='%.4f' y2='%.4f'/>",
				svg_mid_x_pos, svg_mid_y_pos);
		} else {
			struct rnode *parent = node->parent_edge->parent_node;
			struct svg_data *parent_data = parent->data;
			double svg_parent_radius = ROOT_SPACE + (
				r_scale * parent_data->depth);
			double svg_par_x_pos = svg_parent_radius * cos(svg_mid_angle);
			double svg_par_y_pos = svg_parent_radius * sin(svg_mid_angle);
			printf ("<line style='stroke:%s' "
				"stroke-linecap='round' "
				"x1='%.4f' y1='%.4f' x2='%.4f' y2='%.4f'/>",
				color,
				svg_mid_x_pos, svg_mid_y_pos,
				svg_par_x_pos, svg_par_y_pos);
		}

	}
	printf("</g>");
}

/* Prints the node text (labels and lengths) in a <g> element, orthogonal */

void draw_text_ortho (struct rooted_tree *tree, const double h_scale,
		const double v_scale, int align_leaves, double dmax)
{
	printf( "<g"
	       	/* " style='stroke:black;stroke-width:1;" */
	    	/* "font-size:medium;font-weight:normal;font-family:sans'" */
		">"
	      );

	struct list_elem *elem;
	for (elem=tree->nodes_in_order->head; NULL!=elem; elem=elem->next) {
		struct rnode *node = elem->data;
		struct svg_data *node_data = (struct svg_data *) node->data;

		/* For cladograms */
		if (align_leaves && is_leaf(node))
			node_data->depth = dmax;

		char *font_size = is_leaf(node) ?
			leaf_label_font_size : inner_label_font_size ;
		char *color = node_data->color;
		if (NULL == color) color = "black";
		double svg_h_pos = ROOT_SPACE + (h_scale * node_data->depth);
		double svg_mid_pos =
			0.5 * v_scale * (node_data->top+node_data->bottom);

		/* draw label IFF it is nonempty AND requested font size
		 * is not zero */

		if (0 != strcmp(font_size, "0") &&
		    0 != strcmp(node->label, ""))
			printf("<text style='stroke:none;font-size:%s' "
			       "x='%.4f' y='%.4f'>%s</text>",
				font_size, svg_h_pos + LBL_SPACE,
				svg_mid_pos, node->label);

		/* Branch lengths */

		if (! is_root(node)) {
			struct rnode *parent = node->parent_edge->parent_node;
			struct svg_data *parent_data = parent->data;
			double svg_parent_h_pos = ROOT_SPACE + (
				h_scale * parent_data->depth);
			/* Print branch length IFF it is nonempty AND 
			 * requested size is not 0 */
			if (0 != strcmp(branch_length_font_size, "0") &&
			    0 != strcmp(node->parent_edge->length_as_string, "")) {
				printf("<text style='stroke:none;font-size:%s' "
					"x='%4f' y='%4f'>%s</text>",
					branch_length_font_size,
					(svg_h_pos + svg_parent_h_pos) / 2.0,
					edge_length_v_offset + svg_mid_pos,
					node->parent_edge->length_as_string);
			}
		}

	}
	printf("</g>");
}

/* Prints the node text (labels and lengths) in a <g> element, radial */

void draw_text_radial (struct rooted_tree *tree, const double r_scale,
		const double a_scale, int align_leaves, double dmax)
{
	printf( "<g style='stroke:none'>"
	      );

	struct list_elem *elem;
	for (elem=tree->nodes_in_order->head; NULL!=elem; elem=elem->next) {
		struct rnode *node = elem->data;
		struct svg_data *node_data = (struct svg_data *) node->data;

		/* For cladograms */
		if (align_leaves && is_leaf(node))
			node_data->depth = dmax;

		char *font_size = is_leaf(node) ?
			leaf_label_font_size : inner_label_font_size ;
		char *color = node_data->color;
		if (NULL == color) color = "black";
		double radius = ROOT_SPACE + (r_scale * node_data->depth);
		double mid_angle =
			0.5 * a_scale * (node_data->top+node_data->bottom);
		double x_pos;
		double y_pos;

		mid_angle += svg_label_angle_correction;

		double lbl_space;
		if (is_leaf(node))
			lbl_space = LBL_SPACE;
		else
			lbl_space = INNER_LBL_SPACE;

		char *url = NULL;
		if (url_map) url = hash_get(url_map, node->label);

		/* draw label IFF it is nonempty AND requested font size
		 * is not zero */
		if (0 != strcmp(font_size, "0") && 0 != strcmp(node->label, "")) {
			if (url) printf ("<a xlink:href='%s'>", url);
			if (cos(mid_angle) >= 0)  {
				x_pos = (radius+lbl_space) * cos(mid_angle);
				y_pos = (radius+lbl_space) * sin(mid_angle);
				printf("<text style='font-size:%s' "
				       "transform='rotate(%g,%g,%g)' "
				       "x='%.4f' y='%.4f'>%s</text>",
					font_size,
					mid_angle / (2*PI) * 360,
					x_pos, y_pos,
					x_pos, y_pos, node->label);
			}
			else {
				mid_angle += svg_left_label_angle_correction;
				x_pos = (radius+lbl_space) * cos(mid_angle);
				y_pos = (radius+lbl_space) * sin(mid_angle);
				printf("<text style='text-anchor:end;font-size:%s' "
				       "transform='rotate(%g,%g,%g) rotate(180,%g,%g)' "
				       "x='%.4f' y='%.4f'>%s</text>",
					font_size,
					mid_angle / (2*PI) * 360,
					x_pos, y_pos,
					x_pos, y_pos,
					x_pos, y_pos, node->label);
			}
			if (url) printf("</a>");
		}
		/* TODO: add this when node labels work */
		/*
		if (! is_root(node)) {
			struct rnode *parent = node->parent_edge->parent_node;
			struct svg_data *parent_data = parent->data;
			double svg_parent_h_pos = ROOT_SPACE + (
				h_scale * parent_data->depth);
				*/
			/* Print branch length IFF it is nonempty AND 
			 * requested size is not 0 */
			/*
			if (0 != strcmp(branch_length_font_size, "0") &&
			    0 != strcmp(node->parent_edge->length_as_string, "")) {
				printf("<text style='stroke:none;font-size:%s' "
					"x='%4f' y='%4f'>%s</text>",
					branch_length_font_size,
					(svg_h_pos + svg_parent_h_pos) / 2.0,
					edge_length_v_offset + svg_mid_pos,
					node->parent_edge->length_as_string);
			}
		}
		*/
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

void svg_alloc_node_pos(struct rooted_tree *tree) 
{
	struct list_elem *le;
	struct rnode *node;

	for (le = tree->nodes_in_order->head; NULL != le; le = le->next) {
		node = le->data;
		struct svg_data *svgd = malloc(sizeof(struct svg_data));
		if (NULL == svgd) { perror(NULL); exit (EXIT_FAILURE); }
		svgd->top = svgd->bottom = svgd->depth = -1.0;
		svgd->color = NULL;
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

/* Returns the largest power of ten not greater than argument */

double largest_PoT_lte(double arg)
{
	double l10 = log(arg) / log(10);
	double l10_fl = floor(l10);
	return pow(10, l10_fl);
}

/* Draws a scale bar below the tree. Uses a heuristic to manage horizontal
 * space */

void draw_scale_bar(struct rooted_tree *tree, double h_scale,
		double v_scale, double d_max, char *branch_length_unit)
{
	/* Finds the largest power of 10 that is smaller than the tree's depth.
	 * Then draws as many multiples of this length as possible. If no more
	 * than one can be drawn (because it's longer than half the tree's
	 * depth), then we draw tick marks inside it instead. */

	const int big_tick_height = 5; 			/* px */
	const int small_tick_height = 3;		/* px */
	const int units_text_voffset = -13;		/* px */
	const int vsep = 1;				/* px */
	double vpos = leaf_count(tree) * v_scale; 	/* px */
	double pot = largest_PoT_lte(d_max);		/* tree units */
	double scale_length = pot * h_scale;		/* px */

	printf ("<g transform='translate(%d,%g)'>", ROOT_SPACE, vpos); 

	if (2 * pot > d_max) {
		/* print 1/10th tick marks */
		printf ("<path style='fill:none' d='M 0 %d l 0 %d "
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
		/* print as many multiples of 'scale_length' as  will fit */
		printf ("<path d='M 0 %d l 0 %d'/>", vsep, big_tick_height);
		printf ("<text style='font-size:small;text-anchor:stat;"
			"stroke:none' x='0' y='0'>0</text>");
		int i;
		for (i = 0; (i+1)*pot < d_max; i++) {
			printf ("<path transform='translate(%g,0)' "
				"style='fill:none' d='M 0 %d l %g 0 l 0 %d'/>",
				i*scale_length, vsep+big_tick_height,
				scale_length, -big_tick_height);
			printf ("<text transform='translate(%g,0)' "
				"style='font-size:small;text-anchor:end;"
				"stroke:none' x='0' y='0'>%g</text>",
				(i+1)*scale_length, (i+1)*pot);
		}
	}
	printf ("<text style='font-size:small;stroke:none' x='0' y='%d'>"
		"%s</text>", units_text_voffset, branch_length_unit);
	printf ("</g>");
}

void display_svg_tree_orthogonal(struct rooted_tree *tree,
		struct h_data hd, int align_leaves, int with_scale_bar,
		char *branch_length_unit)
{
	double h_scale = -1;
	/* This could get more complicated if we print many trees with
	 * different scales. For now, it's fixed. */
	double v_scale = leaf_vskip;

	if (colormap) set_node_colors(tree);
 	underscores2spaces(tree);

	if (0.0 == hd.d_max ) { hd.d_max = 1; } 	/* one-node trees */
	/* draw nodes */
	h_scale = (graph_width - hd.l_max - ROOT_SPACE - LBL_SPACE) / hd.d_max;
	printf( "<g"
	       	" style='stroke:black;stroke-width:1;"
	    	"font-size:medium;font-weight:normal;font-family:sans'"
		" transform='translate(0,%d)'"
		">", svg_whole_v_shift);
	/* We draw all the tree's branches in an SVG group of their own, to
	 * facilitate editing. */
	draw_branches_ortho(tree, h_scale, v_scale, align_leaves, hd.d_max);
	/* likewise for text */
	draw_text_ortho(tree, h_scale, v_scale, align_leaves, hd.d_max);
	if (with_scale_bar) draw_scale_bar(tree, h_scale, v_scale, hd.d_max,
			branch_length_unit);
	printf ("</g>");
}

void display_svg_tree_radial(struct rooted_tree *tree,
		struct h_data hd, int align_leaves)
{
	double r_scale = -1;
	/* By using 1.9 PI instead of 2 PI, we leave a wedge that shows the
	 * tree's bounds */
	double a_scale = 1.9 * PI / leaf_count(tree); /* radians */

	if (colormap) set_node_colors(tree);
 	underscores2spaces(tree);

	if (0.0 == hd.d_max ) { hd.d_max = 1; } 	/* one-node trees */
	/* draw nodes */
	r_scale = (0.5 * graph_width - CHAR_WIDTH * hd.l_max - 2 * ROOT_SPACE - LBL_SPACE) / hd.d_max;
	printf( "<g transform='translate(%g,%g)'>",
		 	graph_width / 2.0, graph_width / 2.0);
	/* We draw all the tree's branches in an SVG group of their own, to
	 * facilitate editing. */
	draw_branches_radial(tree, r_scale, a_scale, align_leaves, hd.d_max);
	/* likewise for text */
	draw_text_radial(tree, r_scale, a_scale, align_leaves, hd.d_max);
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
