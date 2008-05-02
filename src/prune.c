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

extern int FREE_NODE_DATA;

struct parameters {
	struct llist *labels;
};

struct parameters get_params(int argc, char *argv[])
{
	struct parameters params;

	/*
	int opt_char;
	while ((opt_char = getopt(argc, argv, "l")) != -1) {
		switch (opt_char) {
		case 'l':
			params.try_ingroup = 1;
			break;
		default:
			fprintf (stderr, "Unknown option '-%c'\n", opt_char);
			exit (EXIT_FAILURE);
		}
	}
	*/

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
		fprintf(stderr, "Usage: %s <filename|-> <label> [label+]\n",
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
