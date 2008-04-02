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

void distance_list (struct rooted_tree *tree, struct rnode *origin,
		struct parameters params)
{
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
			// get lca
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
