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
	char *colormap_fname;
	char *leaf_label_font_size;
	char *inner_label_font_size;
};

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
	
	/* parse options and switches */
	while ((opt_char = getopt(argc, argv, "c:e:I:L:sw:")) != -1) {
		switch (opt_char) {
		case 'c':
			params.colormap_fname = optarg;
			break;
		case 'L':
			params.leaf_label_font_size = optarg;
			break;
		case 'I':
			params.inner_label_font_size = optarg;
			break;
		case 's':
			params.svg = TRUE;
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
		fprintf(stderr, "Usage: %s [-w] <filename|->\n",
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
	set_svg_inner_label_font_size(params.inner_label_font_size);
	set_svg_leaf_label_font_size(params.leaf_label_font_size);
	set_svg_colormap_file(params.colormap_fname);
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
