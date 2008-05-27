/* prune.c: remove nodes from tree */

#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "tree.h"
#include "nodemap.h"
#include "parser.h"
#include "to_newick.h"
#include "redge.h"
#include "rnode.h"
#include "link.h"
#include "hash.h"
#include "list.h"

struct parameters {
	struct llist *labels;
};

void help(char *argv[])
{
	printf (
"Removes nodes by label\n"
"\n"
"Synopsis\n"
"--------\n"
"\n"
"%s [-h] <newick trees filename|->\n"
"\n"
"Input\n"
"-----\n"
"\n"
"Argument is the name of a file that contains Newick trees, or '-' (in which\n"
"case trees are read from standard input).\n"
"\n"
"Output\n"
"------\n"
"\n"
"Removes all nodes whose labels are passed on the command line, and prints\n"
"out the modified tree. If removing a node causes its parent to have only\n"
"one child (as is always the case in strictly binary trees), the parent is\n"
"spliced out and the remaining child is attached to its grandparent,\n"
"preserving length.\n"
"\n"
"Options\n"
"-------\n"
"\n"
"    -h: print this message and exit\n"
"\n"
"Assumptions and Limitations\n"
"---------------------------\n"
"\n"
"Labels are assumed to be unique. \n"
"\n"
"Examples\n"
"--------\n"
"\n"
"# Remove humans and gorilla\n"
"%s data/catarrhini Homo Gorilla\n"
"\n"
"# Remove humans, chimp, and gorilla\n"
"%s data/catarrhini Homo Gorilla Pan\n"
"\n"
"# the same, but using the clade's label\n"
"%s data/catarrhini Homininae\n",
	argv[0],
	argv[0],
	argv[0],
	argv[0]
	);
}

struct parameters get_params(int argc, char *argv[])
{
	struct parameters params;

	int opt_char;
	while ((opt_char = getopt(argc, argv, "h")) != -1) {
		switch (opt_char) {
		case 'h':
			help(argv);
			exit (EXIT_SUCCESS);
		default:
			fprintf (stderr, "Unknown option '-%c'\n", opt_char);
			exit (EXIT_FAILURE);
		}
	}

	/* check arguments */
	if ((argc - optind) >= 2)	{
		if (0 != strcmp("-", argv[optind])) {
			FILE *fin = fopen(argv[optind], "r");
			extern FILE *nwsin;
			if (NULL == fin) {
				perror(NULL);
				exit(EXIT_FAILURE);
			}
			nwsin = fin;
		}
		struct llist *lbl_list = create_llist();
		optind++;	/* optind is now index of 1st label */
		for (; optind < argc; optind++) {
			append_element(lbl_list, argv[optind]);
		}
		params.labels = lbl_list;
	} else {
		fprintf(stderr, "Usage: %s [-h] <filename|-> <label> [label+]\n",
				argv[0]);
		exit(EXIT_FAILURE);
	}

	return params;
}

void process_tree(struct rooted_tree *tree, struct llist *labels)
{
	struct hash *lbl2node_map = create_label2node_map(tree->nodes_in_order);
	struct list_elem *elem;

	for (elem = labels->head; NULL != elem; elem = elem->next) {
		char *label = elem->data;
		struct rnode *goner = hash_get(lbl2node_map, label);
		if (NULL == goner) {
			fprintf (stderr, "WARNING: label '%s' not found.\n",
					label);
			continue;
		}
		unlink_rnode (goner);
	}

	destroy_hash(lbl2node_map);
}

int main(int argc, char *argv[])
{
	struct rooted_tree *tree;	
	struct parameters params;
	
	params = get_params(argc, argv);

	while (NULL != (tree = parse_tree())) {
		process_tree(tree, params.labels);
		char *newick = to_newick(tree->root);
		printf ("%s\n", newick);
		free(newick);
		destroy_tree(tree, FREE_NODE_DATA);
	}

	destroy_llist(params.labels);

	return 0;
}
