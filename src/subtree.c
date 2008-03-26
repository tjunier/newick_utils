/* subtree: print subtree from a specified node */
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "tree.h"
#include "parser.h"
#include "nodemap.h"
#include "hash.h"
#include "to_newick.h"
#include "list.h"
#include "lca.h"

/* only one param for now, but who knows? */

struct parameters {
	struct llist *labels;
	int monophyletic;
};

struct parameters get_params(int argc, char *argv[])
{

	struct parameters params;

	int opt_char;
	while ((opt_char = getopt(argc, argv, "m")) != -1) {
		switch (opt_char) {
		case 'm':
			params.monophyletic = 1;
			break;
		default:
			fprintf (stderr, "Unknown option '-%c'\n", opt_char);
			exit(EXIT_FAILURE);
			break; /* ok, not very useful... but I might later decide to ignore the bad option rather than fail. */
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
		fprintf(stderr, "Usage: %s <filename|-> <label> [label+]\n",
				argv[0]);
		exit(EXIT_FAILURE);
	}

	return params;
}

int main(int argc, char *argv[])
{
	struct rooted_tree *tree;	
	struct hash *map;
	struct rnode *subtree_root;
	struct parameters params;
	struct llist *descendants;
	struct list_elem *el;
	
	params = get_params(argc, argv);

	tree = parse_tree();
	map = create_node_map(tree->nodes_in_order);	
	descendants = create_llist();
	for (el = params.labels->head; NULL != el; el = el->next) {
		struct rnode *desc;
		desc = hash_get(map, (char *) el->data);
		if (NULL == desc) {
			fprintf (stderr, "WARNING: label '%s' does not occur in tree\n",
					(char *) el->data);
		} else {
			append_element(descendants, desc);
		}
	}
	subtree_root = lca(tree, descendants);
	if (NULL != subtree_root)
		printf ("%s\n", to_newick(subtree_root));

	destroy_llist(params.labels);

	return 0;
}
