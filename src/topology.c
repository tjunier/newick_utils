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

extern int FREE_NODE_DATA;

struct parameters {
	int show_inner_labels;
	int show_leaf_labels;
	int show_branch_lengths;
};

struct parameters get_params(int argc, char *argv[])
{

	struct parameters params;

	/* defaults */
	params.show_inner_labels = 1;
	params.show_leaf_labels = 1;
	params.show_branch_lengths = 0;

	int opt_char;
	while ((opt_char = getopt(argc, argv, "ILb")) != -1) {
		switch (opt_char) {
		case 'I':
			params.show_inner_labels = 0;
			break;
		case 'L':
			params.show_leaf_labels = 0;
			break;
		case 'b':
			params.show_branch_lengths = 1;
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
		fprintf(stderr, "Usage: %s [-ILb] <filename|->\n", argv[0]);
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
