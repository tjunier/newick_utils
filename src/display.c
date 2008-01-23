/* display - shows Newick as tree graph */
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <sys/resource.h>

#include "parser.h"
#include "tree.h"
#include "text_graph.h"

struct parameters {
	int width;
	int debug;
};

struct parameters get_params(int argc, char *argv[])
{
	struct parameters params;
	int opt_char;

	/* set defaults */
	params.width = 80; 
	params.debug = 0;
	
	/* parse options and switches */
	while ((opt_char = getopt(argc, argv, "dw:")) != -1) {
		switch (opt_char) {
		case 'w':
			params.width = strtod(optarg, NULL);
			if (0 == params.width) {
				fprintf(stderr,
			"Argument to -w must be a positive integer.\n");
				exit(EXIT_FAILURE);
			}
			break;
		case 'd':
			params.debug = 1;
		}
	}
	/* check arguments */
	if (1 == (argc - optind))	{
		if (0 != strcmp("-", argv[optind])) {
			FILE *fin = fopen(argv[optind], "r");
			extern FILE *yyin;
			if (NULL == fin) {
				perror(NULL);
				exit(EXIT_FAILURE);
			}
			yyin = fin;
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

	while (NULL != (tree = parse_tree())) {
		display_tree(tree, params.width);
		destroy_tree(tree);
		/* showmem(); */
	}

	return 0;
}
