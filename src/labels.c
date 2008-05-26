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

struct parameters {
	int show_inner_labels;
	int show_leaf_labels;
	char separator;
};

void help(char *argv[])
{
	printf (
"Extracts the tree's labels\n"
"\n"
"Synopsis\n"
"--------\n"
"\n"
"%s [-hILt] <newick trees filename|->\n"
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
"By default, prints all labels that occur in the tree, in the same order as\n"
"in the Newick, one per line.\n"
"\n"
"Options\n"
"-------\n"
"\n"
"    -h: print this message and exit\n"
"    -I: don't print labels of inner nodes\n"
"    -L: don't print leaf labels\n"
"    -t: TAB-separated - print on a single line, separated by tab stops.\n"
"\n"
"Examples\n"
"--------\n"
"\n"
"# just show labels\n"
"%s data/catarrhini\n"
"\n"
"# count leaves\n"
"%s -I data/catarrhini | wc -l\n",
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
	params.separator = '\n';

	int opt_char;
	while ((opt_char = getopt(argc, argv, "hILt")) != -1) {
		switch (opt_char) {
		case 'h':
			help(argv);
			exit(EXIT_SUCCESS);
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
		fprintf(stderr, "Usage: %s [-hILt] <filename|->\n", argv[0]);
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
