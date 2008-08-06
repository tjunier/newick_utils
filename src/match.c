/* match.c: match a tree to a pattern tree (subgraph) */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
/*
#include <assert.h>
#include <math.h>

#include "list.h"
#include "hash.h"
#include "rnode.h"
#include "redge.h"
#include "node_set.h"
#include "common.h"
*/
#include "parser.h"
#include "to_newick.h"
#include "tree.h"

extern FILE *nwsin;

struct parameters {
	FILE *target_tree_file;
};

void help(char* argv[])
{
	printf(
"Matches a tree to a pattern tree\n"
"\n"
"Synopsis\n"
"--------\n"
"%s <target tree filename|-> <pattern tree>\n"
"\n"
"Input\n"
"-----\n"
"\n"
"The first argument is the name of the file containing the target tree (to which\n"
"support values are to be attributed), or '-' (in which case the tree is read on\n"
"stdin).\n"
"\n"
"The second argument is a pattern tree\n"
"\n"
"Output\n"
"------\n"
"\n"
"Outputs the target tree if the pattern tree is a subgraph of it.\n"
"\n"
"Options\n"
"-------\n"
"\n"
"    -f <file>: reads the pattern from 'file'\n"
"\n"
"Limits & Assumptions\n"
"--------------------\n"
"\n"
"Assumes that the labels are unique in all trees (both target and pattern)\n"
"\n"
"Example\n"
"-------\n"
"\n"
"# Prints trees in data/vrt_gen.nw where Tamias is closer to Homo than it is\n"
"# to Vulpes:\n"
"$ %s data/vrt_gen.nw '((Tamias,Homo),Vulpes);'\n",
	argv[0],
	argv[0]
	      );
}

struct parameters get_params(int argc, char *argv[])
{
	struct parameters params;
	char opt_char;

	/* parse options and switches */
	while ((opt_char = getopt(argc, argv, "hf:")) != -1) {
		switch (opt_char) {
		case 'h':
			help(argv);
			exit(EXIT_SUCCESS);
		/* we keep this for debugging, but not documented */
		case 'f':
			/* read pattern from file */
			break;
		}
	}
	/* get arguments */
	if (2 == (argc - optind))	{
		if (0 != strcmp("-", argv[optind])) {
			FILE *ttf = fopen(argv[optind], "r");
			if (NULL == ttf) {
				perror(NULL);
				exit(EXIT_FAILURE);
			}
			params.target_tree_file = ttf;
		} else {
			params.target_tree_file = stdin;
		}
	} else {
		fprintf(stderr, "Usage: %s [-hlp] <target tree filename|-> <replicates filename>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	return params;
}

int main(int argc, char *argv[])
{
	struct rooted_tree *tree;	
	struct parameters params = get_params(argc, argv);

	nwsin = params.target_tree_file;

	while (NULL != (tree = parse_tree())) {
		char *newick = to_newick(tree->root);
		printf ("%s\n", newick);
		free(newick);
	}

	return 0;
}
