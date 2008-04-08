/* display - shows Newick as tree graph */
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "parser.h"
#include "tree.h"
#include "svg_graph.h"

struct parameters {
	int width;
	int svg;
};

struct parameters get_params(int argc, char *argv[])
{
	struct parameters params;
	int opt_char;

	/* set defaults */
	params.width = -1; 
	params.svg = 0;
	
	/* parse options and switches */
	while ((opt_char = getopt(argc, argv, "e:I:L:sw:")) != -1) {
		switch (opt_char) {
		case 'e':
			set_edge_length_v_offset(-1 * atoi(optarg));
			break;
		case 'L':
			set_leaf_label_font_size(optarg);
			break;
		case 'I':
			set_inner_label_font_size(optarg);
			break;
		case 's':
			params.svg = 1;
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
			params.width = 300;	/* pixels */
		else
			params.width = 80;	/* characters */
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

int main(int argc, char *argv[])
{
	struct rooted_tree *tree;
	struct parameters params;

	params = get_params(argc, argv);

	/* for now, SVG can only handle one tree */
	if (params.svg) {
		tree = parse_tree();
		svg_header();
		display_svg_tree(tree, params.width);
		svg_footer();
		exit(EXIT_SUCCESS);
	}

	while (NULL != (tree = parse_tree())) {
		display_tree(tree, params.width);
		destroy_tree(tree);
		/* showmem(); */
	}

	return 0;
}
