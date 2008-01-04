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

struct parameters {
	struct llist *labels;
};

struct parameters get_params(int argc, char *argv[])
{

	struct parameters params;

	/* TODO: uncomment this when we add switches and options */
	/* parse options and switches */
	/*
	int opt_char;
	while ((opt_char = getopt(argc, argv, "dw:")) != -1) {
		switch (opt_char) {
		case 'w':
			params.width = strtod(optarg, NULL);
			if (0 == params.width) {
				fprintf(stderr,
			"Argument to -w must be a positive integer.\n");
				exit(EXIT_FAILURE);
			}
			break;
		case 'd':
			params.debug = 1;
		}
	}
	*/

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
		fprintf(stderr, "Usage: %s <filename|-> <label> [label+]\n",
				argv[0]);
		exit(EXIT_FAILURE);
	}

	return params;
}

int main(int argc, char *argv[])
{
	struct rooted_tree *tree;	
	struct node_map *map;
	struct rnode *outgroup_root;
	struct parameters params;
	struct llist *descendants;
	struct list_elem *el;
	
	params = get_params(argc, argv);

	tree = parse_tree();
	map = create_node_map(tree->nodes_in_order);	
	descendants = create_llist();
	for (el = params.labels->head; NULL != el; el = el->next) {
		struct rnode *desc;
		desc = get_node_with_label(map, (char *) el->data);
		if (NULL == desc) {
			fprintf (stderr, "WARNING: label '%s' does not occur in tree\n",
					(char *) el->data);
		} else {
			append_element(descendants, desc);
		}
	}
	outgroup_root = lca(tree, descendants);
	if (NULL != outgroup_root) {
		reroot_tree (tree, outgroup_root);
		printf ("%s\n", to_newick(tree->root));
	}

	return 0;
}
