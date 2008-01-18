/* reroot: reroot tree above specified node */
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "tree.h"
#include "parser.h"
#include "nodemap.h"
#include "to_newick.h"
#include "list.h"
#include "lca.h"

#include "rnode.h"

/* return values for reroot() */

const int OK = 0;
const int NO_LCA = 1;
const int LCA_IS_TREE_ROOT = 2;

struct parameters {
	struct llist *labels;
	int try_ingroup;
};

struct parameters get_params(int argc, char *argv[])
{

	struct parameters params;

	params.try_ingroup = 0;

	int opt_char;
	while ((opt_char = getopt(argc, argv, "l")) != -1) {
		switch (opt_char) {
		case 'i':
			params.try_ingroup = 1;
			break;
		}
	}

	/* check arguments */
	if ((argc - optind) >= 2)	{
		if (0 != strcmp("-", argv[optind])) {
			FILE *fin = fopen(argv[optind], "r");
			extern FILE *yyin;
			if (NULL == fin) {
				perror(NULL);
				exit(EXIT_FAILURE);
			}
			yyin = fin;
		}
		struct llist *lbl_list = create_llist();
		optind++;	/* optind is now index of 1st label */
		for (; optind < argc; optind++) {
			append_element(lbl_list, argv[optind]);
		}
		params.labels = lbl_list;
	} else {
		fprintf(stderr, "Usage: %s [-l] <filename|-> <label> [label+]\n",
				argv[0]);
		exit(EXIT_FAILURE);
	}

	return params;
}

/* given the labels of the outgroup nodes, returns the nodes themselves, as a llist. */

struct llist * get_outgroup_nodes(struct rooted_tree *tree, struct llist *labels)
{
	struct node_map *map;
	struct llist *outgroup_nodes;
	struct list_elem *el;

	map = create_node_map(tree->nodes_in_order);	
	outgroup_nodes = create_llist();
	for (el = labels->head; NULL != el; el = el->next) {
		struct rnode *desc;
		desc = get_node_with_label(map, (char *) el->data);
		if (NULL == desc) {
			fprintf (stderr, "WARNING: label '%s' does not occur in tree\n",
					(char *) el->data);
		} else {
			append_element(outgroup_nodes, desc);
		}
	}

	return outgroup_nodes;
}

int reroot(struct rooted_tree *tree, struct llist *outgroup_nodes)
{
	struct rnode *outgroup_root;

	outgroup_root = lca(tree, outgroup_nodes);
	if (NULL != outgroup_root) {
		if (tree->root == outgroup_root) {
			fprintf (stderr, "Outgroup's LCA is tree's root - cannot reroot. Try -l.\n");
			return LCA_IS_TREE_ROOT;
		}
		reroot_tree (tree, outgroup_root);
		printf ("%s\n", to_newick(tree->root));
		return OK;
	} else {
		fprintf (stderr, "Could not find LCA.\n");
		return NO_LCA;
	}
}

int main(int argc, char *argv[])
{
	struct rooted_tree *tree;	
	struct parameters params;
	
	params = get_params(argc, argv);

	while (NULL != (tree = parse_tree())) {
		struct llist *outgroup_nodes = get_outgroup_nodes(tree, params.labels);
		int result = reroot(tree, outgroup_nodes);
		if (LCA_IS_TREE_ROOT == result && params.try_ingroup) {
			struct llist ingroup_nodes = get_ingroup_nodes(tree, params.labels);
			reroot(tree, ingroup_nodes);
		}
		/* TODO: free outgroup_nodes */
	}

	return 0;
}
