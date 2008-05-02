/* nw_labels:  print out the labels of trees */

#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include <string.h>

#include "parser.h"
#include "tree.h"
#include "redge.h"
#include "rnode.h"
#include "list.h"

extern int FREE_NODE_DATA;

struct parameters {
	int show_inner_labels;
	int show_leaf_labels;
	char separator;
};

struct parameters get_params(int argc, char *argv[])
{

	struct parameters params;

	/* defaults */
	params.show_inner_labels = 1;
	params.show_leaf_labels = 1;
	params.separator = '\n';

	int opt_char;
	while ((opt_char = getopt(argc, argv, "ILt")) != -1) {
		switch (opt_char) {
		case 'I':
			params.show_inner_labels = 0;
			break;
		case 'L':
			params.show_leaf_labels = 0;
			break;
		case 't':
			params.separator = '\t';
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
		fprintf(stderr, "Usage: %s [-ILt] <filename|->\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	return params;
}


void process_tree(struct rooted_tree *tree, struct parameters params)
{

	struct list_elem *elem;
	int first_line = 1;

	for (elem = tree->nodes_in_order->head; NULL != elem; elem = elem->next) {
		struct rnode *current = (struct rnode *) elem->data;
		char *label = current->label;

		if (strcmp("", label) == 0)
			continue;

		if (is_leaf(current)) {
			if (params.show_leaf_labels) {
				if (! first_line) putchar(params.separator);
				printf ("%s", label);
				if (first_line) first_line = 0;
			}
		} else {
			if (params.show_inner_labels) {
				if (! first_line) putchar(params.separator);
				printf ("%s", label);
				if (first_line) first_line = 0;
			}
		}
		
	}

	putchar('\n');
}

int main (int argc, char* argv[])
{
	struct rooted_tree *tree;
	struct parameters params;

	params = get_params(argc, argv);

	while ((tree = parse_tree()) != NULL) {
		process_tree(tree, params);
		destroy_tree(tree, FREE_NODE_DATA);
	}

	return 0;
}
