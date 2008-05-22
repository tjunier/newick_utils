/* condense: simplifies tree */
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#include "tree.h"
#include "parser.h"
#include "to_newick.h"

enum actions { PURE_CLADES, STAIR_NODES }; /* not sure we'll keep stair nodes */

struct parameters {
	int action;	/* for now, only condense pure clades */
};

void help(char *argv[])
{
	printf(
"Simplifies a tree by collapsing certain nodes.\n"
"\n"
"Synopsis\n"
"--------\n"
"\n"
"%s [-h] <tree|->\n"
"\n"
"Input\n"
"-----\n"
"\n"
"Argument is either the name of a file that contains one or more trees, or '-',\n"
"in which case the trees are read on stdin.\n"
"\n"
"Output\n"
"------\n"
"\n"
"The tree(s) with pure clades collapsed. A pure clade is a clade in which\n"
"all leaves have the same label, and it is replaced by a leaf of the same\n"
"label: (A,(B,(B,B))); has a pure clade of B, and will be replaced by\n"
"(A,B);. The collapsed clade's support value (if any) is preserved, as is\n"
"its parent edge's length (if specified).\n"
"\n"
"Options\n"
"-------\n"
"\n"
"   -h: prints this message and exits\n"
"\n"
"Example\n"
"-------\n"
"\n"
"# produce a tree of families from a genus tree in which all genus names\n"
"# have been replaced by family names (see nw_rename) - look at\n"
"# data/falconiformes\n"
"$ %s data/falc_families\n",
	argv[0],
	argv[0]
		);
}

struct parameters get_params(int argc, char *argv[])
{

	struct parameters params;

	params.action = PURE_CLADES;

	/* parse options and switches */
	int opt_char;
	while ((opt_char = getopt(argc, argv, "hs")) != -1) {
		switch (opt_char) {
		case 'h':
			help(argv);
			exit(EXIT_SUCCESS);
		case 's':
			/* Not implemented yet - not sure if it will be */
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
		fprintf(stderr, "Usage: %s [-h] <filename|->\n",
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
		collapse_pure_clades(tree);
		char *newick = to_newick(tree->root);
		printf ("%s\n", newick);
		free(newick);
	}

	return 0;
}
