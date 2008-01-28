/* bipart.c: compute bipartitions */

#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "tree.h"
#include "parser.h"
#include "list.h"
#include "hash.h"
#include "rnode.h"
#include "redge.h"
#include "node_set.h"

static struct hash *lbl2num = NULL;
static struct hash *bipart_counts = NULL;
static int num_leaves;

void get_params(int argc, char *argv[])
{
	/* check arguments */
	if (1 == (argc - optind))	{
		if (0 != strcmp("-", argv[optind])) {
			FILE *fin = fopen(argv[optind], "r");
			extern FILE *yyin;
			if (NULL == fin) {
				perror(NULL);
				exit(EXIT_FAILURE);
			}
			yyin = fin;
		}
	} else {
		fprintf(stderr, "Usage: %s <filename|->\n",
				argv[0]);
		exit(EXIT_FAILURE);
	}
}

void init_lbl2num(struct rooted_tree *tree)
{
	struct list_elem *el;
	lbl2num = create_hash(num_leaves);
	int n = 0;

	for (el = tree->nodes_in_order->head; NULL != el; el = el->next) {
		struct rnode *current = (struct rnode *) el->data;
		if (! is_leaf(current)) { continue; }
		int *num = malloc(sizeof(int));
		if (NULL == num) {
			perror(NULL);
			exit(EXIT_FAILURE);
		}
		*num = n;
		hash_set(lbl2num, current->label, num);
		n++;
	}	
}

node_set union_of_child_node_sets(struct rnode *node)
{
	node_set result = create_node_set(num_leaves);
	struct list_elem *el;

	for (el = node->children->head; NULL != el; el = el->next) {
		struct redge *edge = (struct redge *) el->data;
		struct rnode *child = edge->child_node;
		node_set child_node_set = (node_set) child->data;
		node_set_add_set(result, child_node_set, num_leaves);
	}

	return result;
}

void compute_bipartitions(struct rooted_tree *tree)
{
	struct list_elem *el;
	
	for (el = tree->nodes_in_order->head; NULL != el; el = el->next) {
		struct rnode *current = (struct rnode *) el->data;
		node_set set;
		if (is_leaf(current)) {
			int *num = (int *) hash_get(lbl2num, current->label);
			assert (NULL != num);
			set = create_node_set(num_leaves);
			node_set_add(set, *num, num_leaves);
		} else {
			set = union_of_child_node_sets(current);
		}
		current->data = set;
	}
}

void empty_data(struct rooted_tree *tree)
{
	struct list_elem *el;

	for (el = tree->nodes_in_order->head; NULL != el; el = el->next) {
		struct rnode *current = (struct rnode *) el->data;
		free(current->data);
	}
}

void process_tree(struct rooted_tree *tree)
{
	if (NULL == lbl2num) { /* first tree */
		num_leaves = leaf_count(tree);
		init_lbl2num(tree);
		bipart_counts = create_hash(num_leaves);
	}
	compute_bipartitions(tree);
	empty_data(tree);
	destroy_tree_except_data(tree);
}

int main(int argc, char *argv[])
{
	struct rooted_tree *tree;	

	get_params(argc, argv);
	
	while (NULL != (tree = parse_tree())) {
		process_tree(tree);
	}
	return 0;
}
