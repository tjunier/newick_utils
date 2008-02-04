/* collapse: simplifies tree */
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#include "tree.h"
#include "parser.h"
#include "to_newick.h"

enum actions { PURE_CLADES, STAIR_NODES };

struct parameters {
	int action;	
};

struct parameters get_params(int argc, char *argv[])
{

	struct parameters params;

	params.action = PURE_CLADES;

	/* parse options and switches */
	int opt_char;
	while ((opt_char = getopt(argc, argv, "s")) != -1) {
		switch (opt_char) {
		case 's':
			params.action = STAIR_NODES;
			break;
		}
	}

	/* check arguments */
	if ((argc - optind) == 1)	{
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
		fprintf(stderr, "Usage: %s [-s] <filename|->\n",
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

	tree = parse_tree();

	collapse_pure_clades(tree);

	printf ("%s\n", to_newick(tree->root));

	return 0;
}
