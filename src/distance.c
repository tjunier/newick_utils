/* distance: print distances between nodes, in various ways. */
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tree.h"
#include "parser.h"
#include "nodemap.h"
#include "hash.h"
#include "list.h"
#include "lca.h"
#include "node_pos.h"
#include "rnode.h"

enum {FROM_ROOT, FROM_LCA, MATRIX};

struct parameters {
	struct llist *labels;
	int distance_type;
};

struct parameters get_params(int argc, char *argv[])
{

	struct parameters params;

	params.distance_type = FROM_ROOT;

	int opt_char;
	while ((opt_char = getopt(argc, argv, "am")) != -1) {
		switch (opt_char) {
		case 'a':
			params.distance_type = FROM_LCA;
			break;
		case 'm':
			params.distance_type = MATRIX;
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
		fprintf(stderr, "Usage: %s [-ma] <filename|-> <label> [label+]\n",
				argv[0]);
		exit(EXIT_FAILURE);
	}

	return params;
}

/* TODO: maybe this should be moved to lca.c, as it can be resued. */

struct rnode *lca_from_labels(struct rooted_tree *tree, struct llist *labels)
{
	struct hash *node_map = create_node_map(tree->nodes_in_order);
	struct llist *descendant_nodes = create_llist();
	struct list_elem *el;

	for (el = labels->head; NULL != el; el = el->next) {
		char *label = (char *) el->data;
		if (0 == strcmp("", label)) {
			fprintf(stderr, "WARNING: empty label.\n");
			continue;
		}
		struct rnode *desc = hash_get(node_map, label);
		if (NULL == desc) {
			fprintf(stderr, "WARNING: no node with label '%s'.\n",
					label);
			continue;
		}
		append_element(descendant_nodes, desc);
	}

	struct rnode *result = lca(tree, descendant_nodes);

	destroy_hash(node_map);
	destroy_llist(descendant_nodes);

	return result;
}

void distance_list (struct rooted_tree *tree, struct rnode *origin,
		struct parameters params)
{
	/* fill in length data*/
	alloc_node_pos(tree);
	set_node_depth(tree);

	double origin_depth = ((struct node_pos *) origin->data)->depth;

	struct hash *node_map = create_node_map(tree->nodes_in_order);
	struct list_elem *el;
	for (el = params.labels->head; NULL != el; el = el->next) {
		char *label = (char *) el->data;
		if (0 == strcmp("", label)) {
			fprintf(stderr, "WARNING: empty label.\n");
			continue;
		}
		struct rnode *node = hash_get(node_map, label);
		if (NULL == node) {
			fprintf(stderr, "WARNING: no node with label '%s'.\n",
					label);
			continue;
		}
		double node_depth = ((struct node_pos *) node->data)->depth;
		printf ("%g\n", node_depth - origin_depth);
	}
}

void distance_matrix (struct rooted_tree *tree, struct parameters params)
{
}

int main(int argc, char *argv[])
{
	struct rooted_tree *tree;	
	struct parameters params;
	
	params = get_params(argc, argv);

	/* TODO: could take the switch out of the loop, since the distance type
	 * is fixed for the program's lifetime */
	while ((tree = parse_tree()) != NULL) {
		struct rnode *lca;
		switch (params.distance_type) {
		case FROM_ROOT:
			distance_list(tree, tree->root, params);
			break;
		case FROM_LCA:
			lca = lca_from_labels(tree, params.labels);
			distance_list(tree, lca, params);
			break;
		case MATRIX:
			distance_matrix(tree, params);
			break;
		default:
			fprintf (stderr,
				"ERROR: invalid distance type '%d'.\n",
				params.distance_type);
			exit(EXIT_FAILURE);
		}

		destroy_tree(tree);
	}

	destroy_llist(params.labels);

	return 0;
}
