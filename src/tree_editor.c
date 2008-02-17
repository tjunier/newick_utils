#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include "enode.h"
#include "rnode.h"
#include "redge.h"
#include "link.h"
#include "list.h"
#include "tree.h"
#include "parser.h"
#include "to_newick.h"	/* TODO: remove when done */
#include "address_parser.h"
#include "tree_editor_rnode_data.h"

extern int adsdebug;
void address_scanner_set_input(char *);
void address_scanner_clear_input();
int adsparse();

struct enode *expression_root;

struct parameters {
	char * address;
};

struct parameters get_params(int argc, char *argv[])
{
	struct parameters params;

	/* check arguments */
	if (3 == (argc - optind))	{
		if (0 != strcmp("-", argv[optind])) {
			FILE *fin = fopen(argv[optind], "r");
			extern FILE *nwsin;
			if (NULL == fin) {
				perror(NULL);
				exit(EXIT_FAILURE);
			}
			nwsin = fin;
		}
		params.address = argv[optind+1];
	} else {
		fprintf(stderr, "Usage: %s <filename|-> <addr> <act>\n",
				argv[0]);
		exit(EXIT_FAILURE);
	}

	return params;
}

void reverse_parse_order_traversal(struct rooted_tree *tree)
{
	struct list_elem *el;
	struct llist *rev_nodes = llist_reverse(tree->nodes_in_order);
	struct rnode *node;
	struct rnode_data *rndata;

	el = rev_nodes->head;	/* root */
	node = (struct rnode *) el->data;
	rndata = malloc(sizeof(struct rnode_data));
	if (NULL == rndata) { perror(NULL); exit (EXIT_FAILURE); }
	rndata->nb_ancestors = 0;
	rndata->depth = 0;
	node->data = rndata;

	for (el = rev_nodes->head->next; NULL != el; el = el -> next) {
		node = (struct rnode *) el->data;
		struct rnode *parent = node->parent_edge->parent_node;
		rndata = malloc(sizeof(struct rnode_data));
		if (NULL == rndata) { perror(NULL); exit (EXIT_FAILURE); }
		rndata->nb_ancestors = parent->data->nb_ancestors + 1;
		rndata->depth = parent->data->depth +
			atof(node->parent_edge->length_as_string);
	}

	destroy_llist(rev_nodes);
}

void process_tree(struct rooted_tree *tree, struct parameters params)
{
	/* these two traversals fill the node data. */
	// parse_order_traversal();
	reverse_parse_order_traversal(tree);
}

int main(int argc, char* argv[])
{
	struct parameters params = get_params(argc, argv);
	struct rooted_tree *tree;

	adsdebug = 0;
	address_scanner_set_input(params.address);
	adsparse(); /* sets 'expression_root' */
	address_scanner_clear_input();

	while (NULL != (tree = parse_tree())) {
		process_tree(tree, params);
	}

	return 0;
}
