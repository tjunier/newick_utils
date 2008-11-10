/* display - shows Newick as tree graph */
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "common.h"
#include "list.h"
#include "parser.h"
#include "rnode.h"
#include "svg_graph.h"
#include "text_graph.h"
#include "tree.h"
#include "graph_common.h"
#include "svg_graph_common.h"

struct parameters {
	int width;
	int svg;
	FILE *css_map;	
	FILE *url_map;
	char *leaf_label_style;		/* CSS */
	char *inner_label_style;	/* CSS */
	char *edge_label_style;		/* CSS */
	double leaf_vskip;
	int svg_style;		/* radial or orthogonal */
	char *branch_length_unit;
	double label_angle_correction;
	double left_label_angle_correction;
};

void help(char* argv[])
{
	printf(
"Displays a tree as a graph, as text or SVG.\n"
"\n"
"Synopsis\n"
"--------\n"
"\n"
"%s [-aAbchilsuUvw] <tree filename|->\n"
"\n"
"Input\n"
"-----\n"
"\n"
"Argument is the name of the file containing one or more trees,\n"
"or '-' (in which case the tree is read on stdin).\n"
"\n"
"Output\n"
"------\n"
"\n"
"Outputs a graph representing the tree, either as text (default) or\n"
"as SVG (option -s). Underscores in labels are replaced with spaces.\n"
"Trees with no branch lengths are taken to be cladograms and are\n"
"dawn with leaves aligned. Otherwise, tree is assumed to be a\n"
"phylogram and branch lengths are honored and a scale bar is drawn.\n"
"\n"
"Options\n"
"-------\n"
"\n"
"    -a <number>: rotate all labels by this amount (radians, default: 0)\n"
"       [only SVG radial]\n"
"    -A <number>: rotate left-side labels by this amount (radians,"
"       default: 0.0349 (=~ 2°)) [only SVG radial]"
"       [only SVG radial]\n"
"    -b <number>: branch length font size (default: small) [only SVG].\n"    
"       setting to 0 disables printing of branch lengths.\n"
"    -c <css-filename>: use specified CSS map [only SVG]. A CSS map\n"
"       is a text file which specifies a style (CSS) for a clade.\n"
"       Each line has the following structure:\n"
"       <CSS> <label> [label]* \n"
"       <CSS> is a valid CSS style specification (no spaces allowed)\n"
"       e.g. 'font-size:small;font-family:italics;stroke:green'.\n"
"       If there is only one label, this leaf is colored. If there are more, the\n"
"       LCA of the labels and all descendant nodes are colored. If more than one\n"
"       node matches a label, all matching nodes are taken into account in\n"
"       the definition of the LCA.\n"
"       Default: no CSS map, whole tree is black.\n"
"    -h: prints this message and exits\n"
"    -i <number>: inner label font size (default: small) [SVG only]\n"
"       setting to 0 disables printing of inner labels.\n"
"    -l <number>: leaf label font size (default: medium) [SVG only]\n"
"       setting to 0 disables printing of inner labels.\n"
"    -r: draw a radial tree (default: orthogonal) [only SVG]\n"
"    -s: output graph as SVG (default: text). SVG output is currently limited\n"
"       to one tree - any trees beyond the first one are ignored.\n"
"    -u <string>: string is used as unit name for scale bar (ignored\n"
"       if no scale bar is drawn).\n"
"    -U <URL_filename>: use specified URL map [only SVG]. A URL map\n"
"       is a text file which specifies a URL for a label.\n"
"       Each line has the following structure:\n"
"       <label> <URL>\n"
"       Clicking on a label will follow the link (if any).\n"
"    -v <number>: number of pixels between leaves (default: 40) [only SVG]\n"
"    -w <number>: graph should be no wider than <number>, measured in\n"
"       characters for text and pixels for SVG. Defaults: 80 (text),\n"
"       300 (SVG)\n"
"\n"
"Examples\n"
"--------\n"
"\n"
"# display tree as ASCII\n"
"$ %s data/catarrhini\n"
"\n"
"# display tree as SVG, using a color map\n"
"$ %s -s -c data/color.map data/catarrhini\n"
"\n"
"# the same; no branch lengths\n"
"$ %s -s -b 0 -c data/color.map data/catarrhini\n",
	argv[0],
	argv[0],
	argv[0],
	argv[0]
	      );
}

struct parameters get_params(int argc, char *argv[])
{
	struct parameters params;

	params.width =	-1;
	params.svg = FALSE;
	params.css_map = NULL;
	params.url_map = NULL;
	params.leaf_label_style = "font-size:medium;font-family:sans";
	params.inner_label_style = "font-size:small;font-family:sans";
	params.edge_label_style = "font-size:small;font-family:sans";
	params.leaf_vskip = 40.0;
	params.svg_style = SVG_ORTHOGONAL;
	params.branch_length_unit = "";
	params.label_angle_correction = 0.0;
	params.left_label_angle_correction = 0.0;

	int opt_char;
	const int DEFAULT_WIDTH_PIXELS = 300;
	const int DEFAULT_WIDTH_CHARS = 80;
	
	/* parse options and switches */
	while ((opt_char = getopt(argc, argv, "a:A:b:c:hi:l:rsu:U:v:w:")) != -1) {
		switch (opt_char) {
		case 'a':
			params.label_angle_correction = atof(optarg);
			break;
		case 'A':
			params.left_label_angle_correction = atof(optarg);
			break;
		case 'b':
			params.edge_label_style = optarg;
			break;
		case 'c':
			params.css_map = fopen(optarg, "r");
			if (NULL == params.css_map) {
				perror(NULL); exit(EXIT_FAILURE);
			}
			break;
		case 'h':
			help(argv);
			exit(EXIT_SUCCESS);
		case 'i':
			params.inner_label_style = optarg;
			break;
		case 'l':
			params.leaf_label_style = optarg;
			break;
		case 'r':
			params.svg_style = SVG_RADIAL;
			break;
		case 's':
			params.svg = TRUE;
			break;
		case 'u':
			params.branch_length_unit = optarg;
			break;
		case 'U':
			params.url_map = fopen(optarg, "r");
			if (NULL == params.url_map) {
				perror(NULL); exit(EXIT_FAILURE);
			}
			break;
		case 'v':
			params.leaf_vskip = atof(optarg);
			break;
		case 'w':
			params.width = strtod(optarg, NULL);
			if (0 == params.width) {
				fprintf(stderr,
			"Argument to -w must be a positive integer.\n");
				exit(EXIT_FAILURE);
			}
			break;
		}
	}
	/* if width not set, use default (depends on whether SVG or not) */
	if (-1 == params.width) {
		if (params.svg) 
			params.width = DEFAULT_WIDTH_PIXELS;
		else
			params.width = DEFAULT_WIDTH_CHARS;
	}
	/* check arguments */
	if (1 == (argc - optind)) {
		if (0 != strcmp("-", argv[optind])) {
			FILE *fin = fopen(argv[optind], "r");
			extern FILE *nwsin;
			if (NULL == fin) {
				perror(NULL);
				exit(EXIT_FAILURE);
			}
			nwsin = fin;
		}
	} else {
		fprintf(stderr, "Usage: %s [-aAbchilsuUvw] <filename|->\n",
				argv[0]);
		exit(EXIT_FAILURE);
	}

	return params;
}

/* There are so many parameters to an SVG tree that we cannot pass them all
 * to display_svg_tree() without loss of readability and clarity. Therefore
 * svg_graph.c has external variables for those parameters. Since I don't like
 * the idea of directly changing a variable in another module, those variables
 * are static and accessed through setters. This is the purpose of the
 * following function. Note that I could just pass a struct parameters to
 * display_svg_tree(), but then this would make svg_graph.c dependent on struct
 * parameters, increasing coupling. I don't like this. */

void set_svg_parameters(struct parameters params)
{
	set_svg_width(params.width);
	set_svg_leaf_vskip(params.leaf_vskip);
	set_svg_whole_v_shift(20);	/* pixels */
	set_svg_label_angle_correction(params.label_angle_correction);
	set_svg_left_label_angle_correction(params.left_label_angle_correction);
	// TODO: this is not really an SVG param, it should be passed as an
	// argument to display_svg_tree()
	set_svg_style(params.svg_style);	/* radial vs orthogonal */
	set_svg_URL_map_file(params.url_map);
	// TODO: refer to this as "clade CSS", as opposed to node label CSS
	set_svg_CSS_map_file(params.css_map);
	set_svg_leaf_label_style(params.leaf_label_style);
	set_svg_inner_label_style(params.inner_label_style);
	set_svg_edge_label_style(params.edge_label_style);
}

/* Prints an XML comment containing the command line parameters, so that the
 * result is easier to reproduce. */

void svg_run_params_comment(int argc, char *argv[])
{
	int i;

	printf ("\n<!-- " );
	for (i = 0; i < argc; i++) 
		printf ("%s ", argv[i]);
	printf (" -->\n" );
}

int main(int argc, char *argv[])
{
	struct rooted_tree *tree;
	struct parameters params;
	int align_leaves;
	int with_scale_bar;

	params = get_params(argc, argv);

	/* for now, SVG can only handle one tree */
	if (params.svg) {
		set_svg_parameters(params);
		svg_init();
		tree = parse_tree();
		align_leaves = is_cladogram(tree);
		with_scale_bar = !is_cladogram(tree);
		svg_header();
		svg_run_params_comment(argc, argv);
		display_svg_tree(tree, align_leaves, with_scale_bar,
				params.branch_length_unit);
		svg_footer();
		exit(EXIT_SUCCESS);
	}

	while (NULL != (tree = parse_tree())) {
		align_leaves = is_cladogram(tree);
		prettify_labels(tree);
		display_tree(tree, params.width, align_leaves);
		destroy_tree(tree, FREE_NODE_DATA);
	}

	return 0;
}
