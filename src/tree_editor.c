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
#include "to_newick.h"
#include "address_parser.h"
#include "tree_editor_rnode_data.h"

void address_scanner_set_input(char *);
void address_scanner_clear_input();
int adsparse();

enum action { ACTION_DELETE, ACTION_SUBTREE, ACTION_SPLICE_OUT };

struct enode *expression_root;

struct parameters {
	char * address;
	int action;
	int show_tree;
};

struct parameters get_params(int argc, char *argv[])
{
	struct parameters params;

	params.show_tree = 1;

	int opt_char;
	while ((opt_char = getopt(argc, argv, "n")) != -1) {
		switch (opt_char) {
		case 'n':
			params.show_tree = 0;
			break;
		default:
			fprintf (stderr, "Unknown option '-%c'\n", opt_char);
			exit (EXIT_FAILURE);
		}
	}

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
		char action = argv[optind+2][0];
		switch (action) {
		case 's': params.action = ACTION_SUBTREE;
			break;
		case 'o': params.action = ACTION_SPLICE_OUT;
			break;
		case 'd': params.action = ACTION_DELETE;
		  	break;
		default: fprintf(stderr, 
			"Action '%c' is unknown.\n", action);
			 exit(EXIT_FAILURE);
		}
	} else {
		fprintf(stderr, "Usage: %s <filename|-> <addr> <act>\n",
				argv[0]);
		exit(EXIT_FAILURE);
	}

	return params;
}

/* This allocates the rnode_data structure for each node, and fills it with
 * "top-down" data,  i.e. data for which the parent's value needs to be known.
 * */

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
		struct rnode_data *parent_data = (struct rnode_data *) parent->data;
		rndata = malloc(sizeof(struct rnode_data));
		if (NULL == rndata) { perror(NULL); exit (EXIT_FAILURE); }
		rndata->nb_ancestors = parent_data->nb_ancestors + 1;
		rndata->depth = parent_data->depth +
			atof(node->parent_edge->length_as_string);

		node->data = rndata;
	}

	destroy_llist(rev_nodes);
}

/* This fills bottom-up data. Note that it relies on rnode_data being already
 * allocated, which is done in reverse_parse_order_traversal(). Data that does
 * not depend on order is also filled in here. */

void parse_order_traversal(struct rooted_tree *tree)
{
	struct list_elem *el;
	struct rnode *node;
	struct rnode_data *rndata;

	/* NOTE: for now there is no bottom-up data, but this is where it will
	 * be set (e.g., number of descendants, etc) */
	for (el = tree->nodes_in_order->head; NULL != el; el = el -> next) {
		node = (struct rnode *) el->data;
		rndata = (struct rnode_data *) node->data;
		rndata->support = atof(node->label);	
	}
}

void process_tree(struct rooted_tree *tree, struct parameters params)
{
	struct list_elem *el;
	struct rnode *r;
	char *newick;

	/* these two traversals fill the node data. */
	reverse_parse_order_traversal(tree);
	parse_order_traversal(tree);

	for (el = tree->nodes_in_order->head; NULL != el; el = el -> next) {
		struct rnode *current = (struct rnode *) el->data;
		enode_eval_set_current_rnode(current);
		if (eval_enode(expression_root)) {
			switch (params.action) {
			case ACTION_SUBTREE:
				newick = to_newick(current);
				printf("%s\n", newick);
				free(newick);
				break;
			case ACTION_SPLICE_OUT:
				if (is_inner_node(current)) {
					splice_out_rnode(current);
				} else {
					fprintf (stderr, "Warning: tried to splice out non-inner node ('%s')\n", current->label);
				}
				break;
			case ACTION_DELETE:
				r = unlink_rnode(current);
				if (NULL != r) {
					r->parent_edge = NULL;
					tree->root = r;
				} 
				break;
			default: fprintf (stderr,
				"Unknown action %d.\n", params.action);
				 exit(EXIT_FAILURE);
			}
		}	
	}
}

int main(int argc, char* argv[])
{
	struct parameters params = get_params(argc, argv);
	struct rooted_tree *tree;

	address_scanner_set_input(params.address);
	adsparse(); /* sets 'expression_root' */
	address_scanner_clear_input();

	while (NULL != (tree = parse_tree())) {
		process_tree(tree, params);
		if (params.show_tree) {
			char *newick = to_newick(tree->root);
			printf("%s\n", newick);
			free(newick);
		}
		destroy_tree(tree, FREE_NODE_DATA);
	}

	return 0;
}
