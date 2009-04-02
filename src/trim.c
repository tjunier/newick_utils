/* trim.c: trim tree at certain depth (in distance or number of ancestors) */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include "to_newick.h"
#include "tree.h"
#include "parser.h"
/*
#include "nodemap.h"
#include "link.h"
#include "hash.h"
*/
#include "rnode.h"
#include "list.h"
#include "redge.h"

enum {DEPTH_DISTANCE, DEPTH_ANCESTORS};

struct node_data {
	int ancestry_depth;
	double distance_depth;
};

struct parameters {
	int depth_type;
	double threshold;
};

void help(char *argv[])
{
	printf (
"Trims a tree at a certain depth.\n"
"\n"
"Synopsis\n"
"--------\n"
"\n"
"%s [-ah] <newick trees filename|-> <depth>\n"
"\n"
"Input\n"
"-----\n"
"\n"
"The first argument is the name of a file that contains Newick trees, or '-'\n"
"(in which case trees are read from standard input). The second argument is\n"
"the depth at which the tree will be cut.\n"
"\n"
"Output\n"
"------\n"
"\n"
"Returns a tree whose depth is at most that passed as second argument.\n"
"Leaves are shortened but keep their label. Internal nodes are shortened\n"
"and their children are discarded.\n"
"\n"
"Options\n"
"-------\n"
"\n"
"    -a: the depth is expressed in number of ancestors, not distance.\n"
"    -h: print this message and exit\n"
"\n"
"Examples\n"
"--------\n"
"\n"
"# Cut tree at depth 20\n"
"%s data/catarrhini 20\n"
"\n"
"# Cut tree at 3 ancestors or more\n"
"%s -a data/catarrhini 3\n",
	argv[0],
	argv[0],
	argv[0]
	);
}

struct parameters get_params(int argc, char *argv[])
{
	struct parameters params;
	params.depth_type = DEPTH_DISTANCE;

	int opt_char;
	while ((opt_char = getopt(argc, argv, "ah")) != -1) {
		switch (opt_char) {
		case 'a':
			params.depth_type = DEPTH_ANCESTORS;
		case 'h':
			help(argv);
			exit (EXIT_SUCCESS);
		default:
			fprintf (stderr, "Unknown option '-%c'\n", opt_char);
			exit (EXIT_FAILURE);
		}
	}

	/* check arguments */
	if ((argc - optind) == 2)	{
		if (0 != strcmp("-", argv[optind])) {
			FILE *fin = fopen(argv[optind], "r");
			extern FILE *nwsin;
			if (NULL == fin) {
				perror(NULL);
				exit(EXIT_FAILURE);
			}
			nwsin = fin;
		}
		optind++;	/* optind is now index of 2nd arg - depth */
		params.threshold = atof(argv[optind]);
	} else {
		fprintf(stderr, "Usage: %s [-ah] <filename|-> <depth>\n",
				argv[0]);
		exit(EXIT_FAILURE);
	}

	return params;
}

void process_tree(struct rooted_tree *tree, struct parameters params)
{
	struct llist *nodes_in_preorder;
	struct list_elem *elem;
	struct rnode *node;

	nodes_in_preorder = llist_reverse(tree->nodes_in_order);
	node = (struct rnode *) nodes_in_preorder->head->data; /* root */
	struct node_data * ndata = malloc(sizeof(struct node_data));
	if (NULL == ndata) { perror(NULL); exit(EXIT_FAILURE); }
	ndata->distance_depth = 0.0;
	ndata->ancestry_depth = 0;
	node->data = ndata;

	/* This starts just AFTER the root! */
	for (elem = nodes_in_preorder->head->next;
			NULL != elem; elem = elem->next) {
		node = (struct rnode *) elem->data;
		struct redge *parent_edge = node->parent_edge;
		struct rnode *parent = parent_edge->parent_node;
		struct node_data *parent_data = parent->data;

		/* compute this node's depth measures */
		double parent_edge_length = atof(parent_edge->length_as_string);

		/* fill this node's data structure */
		ndata = malloc(sizeof(struct node_data));
		if (NULL == ndata) { perror(NULL); exit(EXIT_FAILURE); }
		ndata->distance_depth = parent_edge_length +
			parent_data->distance_depth;
		ndata->ancestry_depth = 1 + parent_data->ancestry_depth;
		node->data = ndata;

		printf ("%p (%s): %g (%d ancestors)\n", node, node->label,
				((struct node_data*)node->data)->distance_depth,
				((struct node_data*)node->data)->ancestry_depth
		       );

	
	}

}

int main(int argc, char *argv[])
{
	struct rooted_tree *tree;	
	struct parameters params;
	
	params = get_params(argc, argv);

	while (NULL != (tree = parse_tree())) {
		process_tree(tree, params);
		char *newick = to_newick(tree->root);
		printf ("%s\n", newick);
		free(newick);
		destroy_tree(tree, DONT_FREE_NODE_DATA);
	}

	return 0;
}
