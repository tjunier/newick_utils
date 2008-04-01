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
#include "rnode_iterator.h"

/* only one param for now, but who knows? */

struct parameters {
	struct llist *labels;
	int check_monophyly;
};

struct parameters get_params(int argc, char *argv[])
{

	struct parameters params;

	params.check_monophyly = 0;

	int opt_char;
	while ((opt_char = getopt(argc, argv, "m")) != -1) {
		switch (opt_char) {
		case 'm':
			params.check_monophyly = 1;
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

void process_tree(struct rooted_tree *tree, struct parameters params)
{
	struct hash *map;
	struct list_elem *el;

	map = create_node_map(tree->nodes_in_order);	
	struct llist *descendants = create_llist();
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
	int valid_desc_count = descendants->count; /* list gets modified by lca() */
	struct rnode *subtree_root = lca(tree, descendants);

	if (NULL == subtree_root) {
		fprintf (stderr, "WARNING: LCA not found\n");
		return;
	}

	if (params.check_monophyly) {
		struct hash *leaf_map = get_leaf_label_map(subtree_root);
		if (leaf_map->count != valid_desc_count) {
			return;
		}
		for (el = descendants->head; NULL != el; el = el->next) {
			if (NULL == hash_get(leaf_map, (char *) el->data)) {
				return;
			}
		}
	}

	/* monophyly of input labels is verified or not requested */
	printf ("%s\n", to_newick(subtree_root));
}

int main(int argc, char *argv[])
{
	struct rooted_tree *tree;	
	struct parameters params;
	
	params = get_params(argc, argv);

	while (tree = parse_tree()) {
		process_tree(tree, params);
	}

	destroy_llist(params.labels);

	return 0;
}
