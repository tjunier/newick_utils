/* nw_topology - remove structural information such as branch lengths, etc. */

#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include <string.h>

#include "parser.h"
#include "to_newick.h"
#include "tree.h"
#include "redge.h"
#include "rnode.h"
#include "list.h"

struct parameters {
	int show_inner_labels;
	int show_leaf_labels;
	int show_branch_lengths;
};

void help(char *argv[])
{
	printf (
"Discard some or all of a tree's non-structural information\n"
"\n"
"Synopsis\n"
"--------\n"
"\n"
"%s [-bhIL] <newick trees filename|->\n"
"\n"
"Input\n"
"-----\n"
"\n"
"Argument is the name of a file that contains Newick trees, or '-' (in\n"
"which case trees are read from standard input).\n"
"\n"
"Output\n"
"------\n"
"\n"
"By default, prints the input trees without branch lengths, effectively\n"
"creating cladograms.\n"
"\n"
"Options\n"
"-------\n"
"\n"
"    -b: keep branch lengths\n"
"    -h: print this message and exit\n"
"    -I: discard inner node labels\n"
"    -L: discard leaf labels\n"
"\n"
"Examples\n"
"--------\n"
"\n"
"# Make a cladogram of Old-World monkeys and apes by discarding branch\n"
"# length data:\n"
"\n"
"%s data/catarrhini\n"
"\n"
"# Make a purely structural tree (still valid Newick!)\n"
"\n"
"%s -IL data/catarrhini\n",
	argv[0],
	argv[0],
	argv[0]
		);
}

struct parameters get_params(int argc, char *argv[])
{

	struct parameters params;

	/* defaults */
	params.show_inner_labels = 1;
	params.show_leaf_labels = 1;
	params.show_branch_lengths = 0;

	int opt_char;
	while ((opt_char = getopt(argc, argv, "bhIL")) != -1) {
		switch (opt_char) {
		case 'b':
			params.show_branch_lengths = 1;
			break;
		case 'h':
			help(argv);
			exit(EXIT_SUCCESS);
		case 'I':
			params.show_inner_labels = 0;
			break;
		case 'L':
			params.show_leaf_labels = 0;
			break;
		default:
			fprintf (stderr, "Unknown option '-%c'\n", opt_char);
			exit (EXIT_FAILURE);
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
		fprintf(stderr, "Usage: %s [-bhIL] <filename|->\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	return params;
}

void process_tree(struct rooted_tree *tree, struct parameters params)
{

	struct list_elem *elem;
	
	for (elem = tree->nodes_in_order->head; NULL != elem; elem = elem->next) {
		struct rnode *current = (struct rnode *) elem->data;
		if (! params.show_branch_lengths) {
			if (NULL != current->parent_edge) {
				char *length = current->parent_edge->length_as_string;
				length[0] = '\0';
			}
		}
		if (! params.show_inner_labels) {
			if (! is_leaf(current)) {
				char *label = current->label;
				label[0] = '\0';
			}
		}
		if (! params.show_leaf_labels) {
			if (is_leaf(current)) {
				char *label = current->label;
				label[0] = '\0';
			}
		}
	}
}

int main (int argc, char* argv[])
{
	struct rooted_tree *tree;
	struct parameters params;

	params = get_params(argc, argv);

	while ((tree = parse_tree()) != NULL) {
		process_tree(tree, params);
		char *newick = to_newick(tree->root);
		printf ("%s\n", newick);
		free(newick);
		destroy_tree(tree, FREE_NODE_DATA);
	}

	return 0;
}
