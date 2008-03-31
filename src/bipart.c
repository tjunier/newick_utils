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
#include "to_newick.h"

extern FILE *nwsin;

static struct hash *lbl2num = NULL;
static struct hash *bipart_counts = NULL;
static int num_leaves;

struct parameters {
	char * target_tree_filename;
	int show_label_numbers;
	int use_percent;
};

struct parameters get_params(int argc, char *argv[])
{
	struct parameters params;
	char opt_char;

	params.show_label_numbers = 0;
	params.use_percent = 0;

	/* parse options and switches */
	while ((opt_char = getopt(argc, argv, "lp")) != -1) {
		switch (opt_char) {
		case 'l':
			params.show_label_numbers = 1;
			break;
		case 'p':
			params.use_percent = 1;
		}
	}
	/* check arguments */
	if (2 == (argc - optind))	{
		if (0 != strcmp("-", argv[optind])) {
			FILE *fin = fopen(argv[optind], "r");
			if (NULL == fin) {
				perror(NULL);
				exit(EXIT_FAILURE);
			}
			nwsin = fin;
		}
		params.target_tree_filename = argv[optind+1];
	} else {
		fprintf(stderr, "Usage: %s <replicates filename|-> <target tree filename>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	return params;
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

void add_bipart_count(const char *node_set_string)
{
	int *count = hash_get(bipart_counts, node_set_string);
	if (NULL == count) {
		int *num = malloc(sizeof(int));
		if (NULL == num) {
			perror(NULL);
			exit(EXIT_FAILURE);
		}
		*num = 1;
		hash_set(bipart_counts, node_set_string, num);
	} else {
		(*count)++;
	}
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
			char *node_set_string = node_set_to_s(set, num_leaves);
			add_bipart_count(node_set_string);
			free(node_set_string);
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

void show_bipartition_counts()
{
	struct llist *keys = hash_keys(bipart_counts);
	struct list_elem *el;
	assert(NULL != keys);

	for (el = keys->head; NULL != el; el = el->next) {
		char * key = (char *) el->data;
		int * value = (int *) hash_get(bipart_counts, key);
		printf ("%2d\t%s\n", *value, key);
	}

	destroy_llist(keys);
}

/* A wrapper around strcmp() for passing to qsort() */

int qsort_strcmp(const void *s1, const void *s2)
{
	return strcmp(* (char **) s1, * (char **) s2);
}

void show_label_numbers()
{
	struct llist *keys = hash_keys(lbl2num);
	struct list_elem *el;
	int i = 0;
	assert(NULL != keys);
	assert(0 != keys->count);

	char **labels = malloc(keys->count * sizeof(char *));
	if (NULL == labels) {
		perror(NULL);
		exit(EXIT_FAILURE);
	}
	for (el = keys->head; NULL != el; el = el->next) {
		char * key = (char *) el->data;
		labels[i++] = key;
	}
	qsort(labels, keys->count, sizeof(char *), qsort_strcmp);
	for (i = 0; i < keys->count; i++) {
		printf ("%d: %s\n", i, labels[i]);
	}
	destroy_llist(keys);
	free(labels);
}

struct rooted_tree *parse_target_tree(const char *tgt_fn)
{
	struct rooted_tree *tree;

	/* change the lexer's input to the target tree file */
	FILE *fin = fopen(tgt_fn, "r");
	if (NULL == fin) {
		perror(NULL);
		exit(EXIT_FAILURE);
	}
	nwsin = fin;

	tree =  parse_tree();
	if (NULL == tree) {
		fprintf (stderr, "Could not parse target tree.\n");
		exit(EXIT_FAILURE);
	}

	return tree;
}

/* Attributes support values to inner nodes. Argument is the tree, and the
 * number of replicates. If this number is > 0, the counts will be expressed as
 * percentages of it. Otherwise, the counts will be absolute. */

void attribute_support_to_target_tree(struct rooted_tree *tree, int rep_count)
{
	/* TODO: use log10(number of biparts) instead of MAX_COUNT_LENGTH */
	static const int MAX_COUNT_LENGTH = 4;	/* up to 9999, should be enough */
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
			char *node_set_string = node_set_to_s(set, num_leaves);
			int * count_p = hash_get(bipart_counts, node_set_string);
			int count;
			if (NULL == count_p) {
				fprintf(stderr, "WARNING: zero bipart count for %s\n",
						node_set_string);
				count = 0;
			}
			else {
				count = *count_p;
			}
			char * lbl = malloc(MAX_COUNT_LENGTH * sizeof(char));
			if (NULL == lbl) {
				perror(NULL);
				exit(EXIT_FAILURE);
			}
			free(current->label);
			if (rep_count > 0) {
				sprintf (lbl, "%d", 100 * count / rep_count);
			} else {
				sprintf (lbl, "%d", count);
			}
			current->label = lbl;
			free(node_set_string);
		}
		current->data = set;
	}
}

int main(int argc, char *argv[])
{
	struct rooted_tree *tree;	
	struct parameters params = get_params(argc, argv);
	
	/* Builds the bipartition counts hash, and counts the number of
	 * replicates. */
	int rep_count = 0;
	while (NULL != (tree = parse_tree())) {
		process_tree(tree);
		rep_count++;
	}

	if (! params.use_percent) { rep_count = 0; }

	tree = parse_target_tree(params.target_tree_filename);
	attribute_support_to_target_tree(tree, rep_count);
	char *newick = to_newick(tree->root);
	printf ("%s\n", newick);
	free(newick);
	destroy_tree(tree);

	/* Not sure if this is really useful */
	/*
	show_bipartition_counts();
	*/
	if (params.show_label_numbers) show_label_numbers();

	return 0;
}
