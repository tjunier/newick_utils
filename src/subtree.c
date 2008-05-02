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
#include "rnode.h"

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

int is_monophyletic(struct llist *descendants, struct rnode *subtree_root)
{
	int result = 1;

	struct hash *leaf_map = get_leaf_label_map(subtree_root);
	if (leaf_map->count != descendants->count) {
		result = 0;
	}
	struct list_elem *el;
	for (el = descendants->head; NULL != el; el = el->next) {
		char *label = ((struct rnode *) el->data)->label;
		if (NULL == hash_get(leaf_map, label)) {
			result = 0;
			break;
		}
	}

	destroy_hash(leaf_map);

	return result;
}

void process_tree(struct rooted_tree *tree, struct parameters params)
{
	struct hash *map;
	struct list_elem *el;

	map = create_label2node_map(tree->nodes_in_order);	
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
	destroy_hash(map);

	struct llist *desc_clone = shallow_copy(descendants);
	struct rnode *subtree_root = lca(tree, desc_clone);
	free(desc_clone); /* elems freed in lca() */

	if (NULL != subtree_root) {
		if ((! params.check_monophyly) ||
		    (is_monophyletic(descendants, subtree_root))) {
			/* monophyly of input labels is verified or not requested */
			char *newick = to_newick(subtree_root);
			printf ("%s\n", newick);
			free(newick);
		}
	} else {
		fprintf (stderr, "WARNING: LCA not found\n");
	}

	destroy_llist(descendants);

}

int main(int argc, char *argv[])
{
	struct rooted_tree *tree;	
	struct parameters params;
	
	params = get_params(argc, argv);

	while ((tree = parse_tree()) != NULL) {
		process_tree(tree, params);
		destroy_tree(tree, DONT_FREE_NODE_DATA);
	}

	destroy_llist(params.labels);

	return 0;
}
