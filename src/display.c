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

struct parameters {
	int width;
	int svg;
	char *colormap_fname;	/* TODO: might store FILE* instead of name */
	char *leaf_label_font_size;
	char *inner_label_font_size;
	char *branch_length_font_size;
	double leaf_vskip;
	int svg_style;	/* radial or orthogonal */
};

void help(char* argv[])
{
	printf(
"Displays a tree as a graph, as text or SVG.\n"
"\n"
"Synopsis\n"
"--------\n"
"\n"
"%s [-bchilsvw] <tree filename|->\n"
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
"\n"
"Options\n"
"-------\n"
"\n"
"    -b <number>: branch length font size (default: small) [only SVG].\n"    
"       setting to 0 disables printing of branch lengths.\n"
"    -c <colormap-file>: use specified colormap [only SVG]. A colormap is a text\n"    
"       file which specifies a color for a clade. Each line has the following\n"
"       structure:\n"
"       <color> <label> [label]* \n"
"       <color> is a valid SVG color, e.g. 'blue' or '#ff00a1' (see\n"
"       http://www.w3.org/TR/SVG11/types.html#DataTypeColor).\n"
"       If there is only one label, this leaf is colored. If there are more, the\n"
"       LCA of the labels and all descendant nodes are colored.\n"
"       Default: no colormap, whole tree is black.\n"
"    -h: prints this message and exits\n"
"    -i <number>: inner label font size (default: small) [SVG only]\n"
"       setting to 0 disables printing of inner labels.\n"
"    -l <number>: leaf label font size (default: small) [SVG only]\n"
"       setting to 0 disables printing of inner labels.\n"
"    -r: draw a radial tree (default: orthogonal) [only SVG]\n"
"    -s: output graph as SVG (default: text). SVG output is currently limited\n"
"       to one tree - any trees beyond the first one are ignored.\n"
"    -w <number>: graph should be no wider than <number>, measured in\n"
"       characters for text and pixels for SVG. Defaults: 80 (text),\n"
"       300 (SVG)\n"
"    -v <number>: number of pixels between leaves (default: 40) [only SVG]\n"
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
	int opt_char;
	const int DEFAULT_WIDTH_PIXELS = 300;
	const int DEFAULT_WIDTH_CHARS = 80;

	/* set defaults */
	params.width = -1; 
	params.svg = FALSE;
	params.colormap_fname = NULL;
	params.leaf_label_font_size = "medium";
	params.inner_label_font_size = "small";
	params.branch_length_font_size = "small";
	params.leaf_vskip = 40.0;
	params.svg_style = SVG_ORTHOGONAL;
	
	/* parse options and switches */
	while ((opt_char = getopt(argc, argv, "b:c:hi:l:rsv:w:")) != -1) {
		switch (opt_char) {
		case 'b':
			params.branch_length_font_size = optarg;
			break;
		case 'c':
			params.colormap_fname = optarg;
			break;
		case 'h':
			help(argv);
			exit(EXIT_SUCCESS);
		case 'i':
			params.inner_label_font_size = optarg;
			break;
		case 'l':
			params.leaf_label_font_size = optarg;
			break;
		case 'r':
			params.svg_style = SVG_RADIAL;
			break;
		case 's':
			params.svg = TRUE;
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
	if (1 == (argc - optind))	{
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
		fprintf(stderr, "Usage: %s [-bchilsvw] <filename|->\n",
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
	set_svg_leaf_label_font_size(params.leaf_label_font_size);
	set_svg_inner_label_font_size(params.inner_label_font_size);
	set_svg_branch_length_font_size(params.branch_length_font_size);
	set_svg_colormap_file(params.colormap_fname);
	set_svg_leaf_vskip(params.leaf_vskip);
	set_svg_whole_v_shift(20);	/* pixels */
	set_svg_style(params.svg_style);	/* radial vs orthogonal */
}

void underscores2spaces(struct rooted_tree *tree)
{
	struct list_elem *el;
	for (el = tree->nodes_in_order->head; NULL != el; el = el->next) {
		struct rnode *current = el->data;
		char *p;
		for (p = current->label; '\0' != *p; p++)
			if ('_' == *p)
				*p = ' ';
	}
}

int main(int argc, char *argv[])
{
	struct rooted_tree *tree;
	struct parameters params;
	int align_leaves;

	params = get_params(argc, argv);

	/* for now, SVG can only handle one tree */
	if (params.svg) {
		set_svg_parameters(params);
		svg_init();
		tree = parse_tree();
		align_leaves = is_cladogram(tree);
		svg_header();
		display_svg_tree(tree, align_leaves);
		svg_footer();
		exit(EXIT_SUCCESS);
	}

	while (NULL != (tree = parse_tree())) {
		align_leaves = is_cladogram(tree);
		underscores2spaces(tree);
		display_tree(tree, params.width, align_leaves);
		destroy_tree(tree, FREE_NODE_DATA);
	}

	return 0;
}
