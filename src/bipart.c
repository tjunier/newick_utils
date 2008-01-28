/* bipart.c: compute bipartitions */

#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "tree.h"
#include "parser.h"
#include "list.h"
#include "hash.h"
#include "rnode.h"
#include "redge.h"
#include "node_set.h"

struct hash *lbl2num = NULL;
struct hash *bipart_counts = NULL;

void get_params(int argc, char *argv[])
{
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
		fprintf(stderr, "Usage: %s <filename|->\n",
				argv[0]);
		exit(EXIT_FAILURE);
	}
}

void init_lbl2num(struct rooted_tree *tree)
{
	
}

void process_tree(struct rooted_tree *tree)
{
	destroy_tree_except_data(tree);
}

int main(int argc, char *argv[])
{
	struct rooted_tree *tree;	

	get_params(argc, argv);
	
	while (NULL != (tree = parse_tree())) {
		process_tree(tree);
		if (NULL == lbl2num) init_lbl2num(tree);
	}
	return 0;
}
