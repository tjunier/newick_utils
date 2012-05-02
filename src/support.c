/* 

Copyright (c) 2009 Thomas Junier and Evgeny Zdobnov, University of Geneva
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice,
    this list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright notice,
    this list of conditions and the following disclaimer in the documentation
    and/or other materials provided with the distribution.
* Neither the name of the University of Geneva nor the names of its
    contributors may be used to endorse or promote products derived from this
    software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/
/* bipart.c: compute bipartitions */

#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <stdbool.h>

#include "tree.h"
#include "parser.h"
#include "list.h"
#include "hash.h"
#include "rnode.h"
#include "node_set.h"
#include "to_newick.h"
#include "common.h"

extern FILE *nwsin;

static struct hash *lbl2num = NULL;
static struct hash *bipart_counts = NULL;
static int num_leaves;

struct parameters {
	FILE * target_tree_file;
	FILE * rep_trees_file;
	bool show_label_numbers;
	bool use_percent;
};

void help(char* argv[])
{
	printf(
"Attributes bootstrap support values to a tree.\n"
"\n"
"Synopsis\n"
"--------\n"
"%s [-ph] <target tree filename|-> <replicate trees filename>\n"
"\n"
"Input\n"
"-----\n"
"\n"
"The first argument is the name of the file containing the target tree (to which\n"
"support values are to be attributed), or '-' (in which case the tree is read on\n"
"stdin).\n"
"\n"
"The second argument is the name of the file containing the replicates.\n"
"\n"
"Output\n"
"------\n"
"\n"
"Outputs the target tree, with a bipartition frequencies as inner node labels.\n"
"\n"
"Options\n"
"-------\n"
"\n"
"    -h: prints this message and exits\n"
"    -p: prints values as percentages (default: absolute frequencies)\n"
"\n"
"Limits & Assumptions\n"
"--------------------\n"
"\n"
"Assumes that the trees have the same number of leaves, and that all trees have\n"
"the same leaf labels. Behaviour is undefined if this is violated.\n"
"\n"
"Example\n"
"-------\n"
"\n"
"# Attributes bipartition counts to data/HRV.nw, based on 20 replicates\n"
"# stored in data/HRV_20reps.nw\n"
"$ %s data/HRV.nw data/HRV_20reps.nw\n",
	argv[0],
	argv[0]
	      );
}

struct parameters get_params(int argc, char *argv[])
{
	struct parameters params;
	char opt_char;

	params.show_label_numbers = false;
	params.use_percent = false;

	/* parse options and switches */
	while ((opt_char = getopt(argc, argv, "hlp")) != -1) {
		switch (opt_char) {
		case 'h':
			help(argv);
			exit(EXIT_SUCCESS);
		/* we keep this for debugging, but not documented */
		case 'l':
			params.show_label_numbers = true;
			break;
		case 'p':
			params.use_percent = true;
			break;
		}
	}
	/* get arguments */
	if (2 == (argc - optind))	{
		if (0 != strcmp("-", argv[optind])) {
			FILE *ttf = fopen(argv[optind], "r");
			if (NULL == ttf) {
				perror(NULL);
				exit(EXIT_FAILURE);
			}
			params.target_tree_file = ttf;
		} else {
			params.target_tree_file = stdin;
		}
		FILE *rtf = fopen(argv[optind+1], "r");
		if (NULL == rtf) {
			perror(NULL);
			exit(EXIT_FAILURE);
		}
		params.rep_trees_file = rtf;
	} else {
		fprintf(stderr, "Usage: %s [-hlp] <target tree filename|-> <replicates filename>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	return params;
}

int init_lbl2num(struct rooted_tree *tree)
{
	struct list_elem *el;
	lbl2num = create_hash(num_leaves);
	if (NULL == lbl2num) return FAILURE;
	int n = 0;

	for (el = tree->nodes_in_order->head; NULL != el; el = el->next) {
		struct rnode *current = (struct rnode *) el->data;
		if (! is_leaf(current)) { continue; }
		int *num = malloc(sizeof(int));
		if (NULL == num) return FAILURE;
		*num = n;
		if (! hash_set(lbl2num, current->label, num))
			return FAILURE;
		n++;
	}	

	return SUCCESS;
}

/* This could also be done with node_set_union(), but it's easier to write with
 * 'result' as an accumulator. */

node_set union_of_child_node_sets(struct rnode *node)
{
	node_set result = create_node_set(num_leaves);
	if (NULL == result) { perror(NULL); exit(EXIT_FAILURE); }
	struct rnode *curr;

	for (curr=node->first_child; NULL != curr; curr = curr->next_sibling) {
		node_set child_node_set = (node_set) curr->data;
		node_set_add_set(result, child_node_set, num_leaves);
	}

	return result;
}

int add_bipart_count(const char *node_set_string)
{
	int *count = hash_get(bipart_counts, node_set_string);
	if (NULL == count) {
		int *num = malloc(sizeof(int));
		if (NULL == num) {
			perror(NULL);
			exit(EXIT_FAILURE);
		}
		*num = 1;
		if (! hash_set(bipart_counts, node_set_string, num))
			return FAILURE;
	} else {
		(*count)++;
	}

	return SUCCESS;
}

void compute_bipartitions(struct rooted_tree *tree)
{
	struct list_elem *el;
	
	for (el = tree->nodes_in_order->head; NULL != el; el = el->next) {
		struct rnode *current = (struct rnode *) el->data;
		node_set set;
		if (is_leaf(current)) {
			int *num = hash_get(lbl2num, current->label);
			if (NULL == num) {
				fprintf(stderr,
					"Label '%s' not found - aborting\n",
					current->label);
				exit(EXIT_FAILURE);
			}
			set = create_node_set(num_leaves);
			if (NULL == set) {perror(NULL); exit(EXIT_FAILURE);}
			node_set_add(set, *num, num_leaves);
		} else {
			set = union_of_child_node_sets(current);
			char *node_set_string = node_set_to_s(set, num_leaves);
			if (NULL == node_set_string) {
				perror(NULL);
				exit(EXIT_FAILURE);
			}
			if (! add_bipart_count(node_set_string)) {
				perror(NULL);
				exit(EXIT_FAILURE);
			}
			free(node_set_string);
		}
		current->data = set;
	}
}

int process_tree(struct rooted_tree *tree)
{
	if (NULL == lbl2num) { /* first tree */
		num_leaves = leaf_count(tree);
		if (! init_lbl2num(tree)) return FAILURE;
		bipart_counts = create_hash(num_leaves);
		if (NULL == bipart_counts) return FAILURE;
	}
	compute_bipartitions(tree);
	destroy_all_rnodes(NULL);
	destroy_tree(tree);

	return SUCCESS;
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

/* Attributes support values to inner nodes. Argument is the tree, and the
 * number of replicates. If this number is > 0, the counts will be expressed as
 * percentages of it. Otherwise, the counts will be absolute. */

void attribute_support_to_target_tree(struct rooted_tree *tree, int rep_count)
{
	/* # of digits for bipart count - need this to know string length */
	/* e.g. if count = 143, need 3 characters */
	int max_count_length;
       	if (0 == rep_count)	/* percent: 3 chars or less */
		max_count_length = 3;
	else
		max_count_length = log10(bipart_counts->count) + 1;	
	struct list_elem *el;
	
	for (el = tree->nodes_in_order->head; NULL != el; el = el->next) {
		struct rnode *current = (struct rnode *) el->data;
		node_set set;
		if (is_leaf(current)) {
			int *num = (int *) hash_get(lbl2num, current->label);
			assert (NULL != num);
			set = create_node_set(num_leaves);
			if (NULL == set) {perror(NULL); exit(EXIT_FAILURE);}
			node_set_add(set, *num, num_leaves);
		} else {
			set = union_of_child_node_sets(current);
			char *node_set_string = node_set_to_s(set, num_leaves);
			if (NULL == node_set_string) {
				perror(NULL);
				exit(EXIT_FAILURE);
			}
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
			/* + 1 for '\0' */
			char * lbl = malloc(max_count_length * sizeof(char) + 1);
			if (NULL == lbl) {
				perror(NULL);
				exit(EXIT_FAILURE);
			}
			free(current->label);
			if (rep_count > 0) {	/* percent */
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
	
	/* Build the bipartition counts hash, and counts the number of
	 * replicates. */
	nwsin = params.rep_trees_file;
	int rep_count = 0;
	while (NULL != (tree = parse_tree())) {
		if (! process_tree(tree)) {
			fprintf(stderr, "Could not process tree "
				"(memory error) - exiting.\n");
			exit(EXIT_FAILURE);
		}
		rep_count++;
	}

	if (! params.use_percent) { rep_count = 0; }

	/* Attribute counts to the target trees */
	nwsin = params.target_tree_file;
	while ((tree = parse_tree()) != NULL) {
		attribute_support_to_target_tree(tree, rep_count);
		char *newick = to_newick(tree->root);
		printf ("%s\n", newick);
		free(newick);
		if (params.show_label_numbers) show_label_numbers();
		destroy_all_rnodes(NULL);
		destroy_tree(tree);
	}

	fclose(params.target_tree_file);
	fclose(params.rep_trees_file);

	return 0;
}
