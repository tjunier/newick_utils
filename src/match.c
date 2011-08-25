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
/* match.c: match a tree to a pattern tree (subgraph) */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include <stdbool.h>

#include "parser.h"
#include "to_newick.h"
#include "tree.h"
#include "order_tree.h"
#include "hash.h"
#include "list.h"
#include "rnode.h"
#include "link.h"
#include "nodemap.h"
#include "common.h"
#include "rnode_iterator.h"
#include "masprintf.h"

#ifdef DEBUG_MATCH
#define DEBUG 1
#else
#define DEBUG 0
#endif

void newick_scanner_set_string_input(char *);
void newick_scanner_clear_string_input();
void newick_scanner_set_file_input(FILE *);

struct parameters {
	char *pattern;
	FILE *target_trees;
	bool reverse;
};

void help(char* argv[])
{
	printf(
"Matches a tree to a pattern tree\n"
"\n"
"Synopsis\n"
"--------\n"
"%s [-v] <target tree filename|-> <pattern tree>\n"
"\n"
"Input\n"
"-----\n"
"\n"
"The first argument is the name of the file containing the target tree (to\n"
"which support values are to be attributed), or '-' (in which case the tree\n"
"is read on stdin).\n"
"\n"
"The second argument is a pattern tree\n"
"\n"
"Output\n"
"------\n"
"\n"
"Outputs the target tree if the pattern tree is a subgraph of it.\n"
"\n"
"Options\n"
"-------\n"
"\n"
"    -v: prints tree which do NOT match the pattern.\n"
"\n"
"Limits & Assumptions\n"
"--------------------\n"
"\n"
"Assumes that the labels are leaf labels, and that they are unique in\n"
"all trees (both target and pattern)\n"
"\n"
"Example\n"
"-------\n"
"\n"
"# Prints trees in data/vrt_gen.nw where Tamias is closer to Homo than it is\n"
"# to Vulpes:\n"
"$ %s data/vrt_gen.nw '((Tamias,Homo),Vulpes);'\n"
"\n"
"# Prints trees in data/vrt_gen.nw where Tamias is NOT closer to Homo than it is\n"
"# to Vulpes:\n"
"$ %s -v data/vrt_gen.nw '((Tamias,Homo),Vulpes);'\n",

	argv[0],
	argv[0],
	argv[0]
	      );
}

struct parameters get_params(int argc, char *argv[])
{
	struct parameters params;
	char opt_char;


	params.reverse = false;

	/* parse options and switches */
	while ((opt_char = getopt(argc, argv, "hv")) != -1) {
		switch (opt_char) {
		case 'h':
			help(argv);
			exit(EXIT_SUCCESS);
		/* we keep this for debugging, but not documented */
		case 'v':
			params.reverse = true;
			break;
		}
	}
	/* get arguments */
	if (2 == (argc - optind))	{
		if (0 != strcmp("-", argv[optind])) {
			FILE *fin = fopen(argv[optind], "r");
			if (NULL == fin) {
				perror(NULL);
				exit(EXIT_FAILURE);
			}
			params.target_trees = fin;
		} else {
			params.target_trees = stdin;
		}
		params.pattern = argv[optind+1];
	} else {
		fprintf(stderr, "Usage: %s [-hv] <target trees filename|-> <pattern>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	return params;
}

/* A debugging function */

void show_node_children_numbers(struct rooted_tree *tree)
{
	struct list_elem *el;
	for (el = tree->nodes_in_order->head; NULL != el; el = el->next) {
		struct rnode *current = el->data;
		printf ("node %p (%s): %d children\n", current,
			current->label, children_count(current));
	}
}

/* Get pattern tree and order it */

struct rooted_tree *get_ordered_pattern_tree(char *pattern)
{
	struct rooted_tree *pattern_tree;

	newick_scanner_set_string_input(pattern);
	pattern_tree = parse_tree();
	if (NULL == pattern_tree) {
		fprintf (stderr, "Could not parse pattern tree '%s'\n", pattern);
		exit(EXIT_FAILURE);
	}
	newick_scanner_clear_string_input();

	if (!order_tree_lbl(pattern_tree)) {
		perror(NULL);
		exit(EXIT_FAILURE);
	}

	return pattern_tree;
}

/* Removes inner labels, since match operates on leaf labels only (this might
 * change if operating on internal labels proves useful). */

void remove_inner_node_labels(struct rooted_tree *target_tree)
{
	struct list_elem *el;

	for (el=target_tree->nodes_in_order->head; NULL != el; el=el->next) {
		struct rnode *current = el->data;
		if (is_leaf(current)) continue;
		free(current->label);
		// We need to allocate dynamically, since this will later be
		// passed to free().
		current->label = strdup("");
	}
	/* The tree topology was not changed, so no need to recompute the node
	 * list */
}

/* Removes all nodes in target tree whose labels are not found in the 'kept'
 * hash */

void prune_extra_labels(struct rooted_tree *target_tree, struct hash *kept)
{
	struct list_elem *el;

	for (el=target_tree->nodes_in_order->head; NULL != el; el=el->next) {
		struct rnode *current = el->data;
		char *label = current->label;
		if (0 == strcmp("", label)) continue;
		if (is_root(current)) continue;
		if (NULL == hash_get(kept, current->label)) {
			/* not in 'kept': remove */
			struct rnode *unlink_root;
			enum unlink_rnode_status result = unlink_rnode(current);
			switch(result) {
			case UNLINK_RNODE_DONE:
				break;
			case UNLINK_RNODE_ROOT_CHILD:
				unlink_root = get_unlink_rnode_root_child(); 
				unlink_root->parent = NULL;
				target_tree->root = unlink_root;
				break;
			case UNLINK_RNODE_ERROR:
				fprintf (stderr, "Memory error - "
						"exiting.\n");
				exit(EXIT_FAILURE);
			default:
				assert(0); /* programmer error */
			}
		}
	}

	/* Topology may have been altered: recompute nodes_in_order. */
	destroy_llist(target_tree->nodes_in_order);
	target_tree->nodes_in_order = get_nodes_in_order(target_tree->root);
}

void prune_empty_labels(struct rooted_tree *target_tree)
{
	struct list_elem *el;
	for (el=target_tree->nodes_in_order->head; NULL != el; el=el->next) {
		struct rnode *current = el->data;
		char *label = current->label;
		/* If none of the labels in the target tree are found in the
		 * pattern, we end up with a null tree consisting of nothing
		 * but an unlabeled root. We can't unlink it, hence the test.
		 * */
		if (is_leaf(current) && ! is_root(current)) {
			if (0 == strcmp("", label)) {
				struct rnode *unlink_root;
				enum unlink_rnode_status result =
					unlink_rnode(current);
				switch(result) {
				case UNLINK_RNODE_DONE:
					break;
				case UNLINK_RNODE_ROOT_CHILD:
					unlink_root =
						get_unlink_rnode_root_child(); 
					unlink_root->parent = NULL;
					target_tree->root = unlink_root;
					break;
				case UNLINK_RNODE_ERROR:
					perror(NULL);
					exit(EXIT_FAILURE);
				default:
					assert(0); /* programmer error */
				}
			}
		}
	}

	/* Topology may have been altered: recompute nodes_in_order. */
	destroy_llist(target_tree->nodes_in_order);
	target_tree->nodes_in_order = get_nodes_in_order(target_tree->root);
}

void remove_branch_lengths(struct rooted_tree *target_tree)
{
	struct list_elem *el;

	for (el = target_tree->nodes_in_order->head; NULL != el; el = el->next) {
		struct rnode *current = el->data;
		if (strcmp("", current->edge_length_as_string) != 0) {
			free(current->edge_length_as_string);
			// We need to allocate dynamically, since this will
			// later be passed to free():
			// cur_edge->length_as_string = "" *WRONG!*
			current->edge_length_as_string = strdup("");
		}
	}
	/* The tree topology was not changed, so no need to recompute
	 * nodes_in_order */
}

void remove_knee_nodes(struct rooted_tree *tree)
{
	struct list_elem *el;

	for (el = tree->nodes_in_order->head; NULL != el; el = el->next) {
		struct rnode *current = el->data;
		if (is_inner_node(current))
			if (1 == children_count(current)) { 
				if (! splice_out_rnode(current)) {
					perror(NULL);
					exit(EXIT_FAILURE);
				}
				/* NOTE: don't destroy the current node here.
				 * This is taken care of later in main(). */
			}
	}

	/* If the root has only one child, make that child the new root */
	if (1 == children_count(tree->root)) 
		tree->root = tree->root->first_child;

	/* Topology may have been altered: recompute nodes_in_order. */
	struct llist *nodes_in_order = get_nodes_in_order(tree->root);
	if (NULL == nodes_in_order) { perror(NULL); exit(EXIT_FAILURE); }
	destroy_llist(tree->nodes_in_order);
	tree->nodes_in_order = nodes_in_order;
}

void process_tree(struct rooted_tree *tree, struct hash *pattern_labels,
		char *pattern_newick, struct parameters params)
{
	/* NOTE: whenever I alter the tree structure, I rebuild nodes_in_order
	 * as soon as possible. Then I no longer need to guard against this
	 * list being invalid. */

	/* In this case I stick with the old, recursive to_newick, because it
	 * directly returns a char* */
	char *original_newick = to_newick(tree->root);
	remove_inner_node_labels(tree);
	prune_extra_labels(tree, pattern_labels);
	prune_empty_labels(tree);
	remove_knee_nodes(tree);
	remove_branch_lengths(tree);	
	if (! order_tree_lbl(tree)) { perror(NULL); exit(EXIT_FAILURE); }
	/* Ordering does not change topology, but it does change node
	 * oridering, hence nodes_in_order must be recomputed. */
	struct llist *nodes_in_order = get_nodes_in_order(tree->root);
	if (NULL == nodes_in_order) { perror(NULL); exit(EXIT_FAILURE); }
	destroy_llist(tree->nodes_in_order);
	tree->nodes_in_order = nodes_in_order;

	char *processed_newick = to_newick(tree->root);
	int match = (0 == strcmp(processed_newick, pattern_newick));
	match = params.reverse ? !match : match;
	if (match) printf ("%s\n", original_newick);
	free(processed_newick);
	free(original_newick);
}

int main(int argc, char *argv[])
{
	struct rooted_tree *pattern_tree;	
	struct rooted_tree *tree;	
	char *pattern_newick;
	struct hash *pattern_labels;

	struct parameters params = get_params(argc, argv);

	pattern_tree = get_ordered_pattern_tree(params.pattern);
	pattern_newick = to_newick(pattern_tree->root);
	pattern_labels = create_label2node_map(pattern_tree->nodes_in_order);

	/* get_ordered_pattern_tree() causes a tree to be read from a string,
	 * which means that we must now tell the lexer to change its input
	 * source. It's not enough to just set the external FILE pointer
	 * 'nwsin' to standard input or the user-supplied file, apparently:
	 * this would segfault. */
	newick_scanner_set_file_input(params.target_trees);

	while (NULL != (tree = parse_tree())) {
		process_tree(tree, pattern_labels, pattern_newick, params);
		destroy_tree(tree);
	}

	destroy_hash(pattern_labels);
	free(pattern_newick);
	destroy_all_rnodes(NULL);
	destroy_tree(pattern_tree);
	return 0;
}
