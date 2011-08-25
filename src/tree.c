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
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <regex.h>
#include <stdbool.h>

#include "tree.h"
#include "link.h"
#include "rnode.h"
#include "list.h"
#include "nodemap.h"
#include "hash.h"
#include "rnode_iterator.h"
#include "common.h"

const int FREE_NODE_DATA = 1;
const int DONT_FREE_NODE_DATA = 0;


/* 'outgroup' is the node which will be the outgroup after rerooting. */

int reroot_tree(struct rooted_tree *tree, struct rnode *outgroup)
{
	struct rnode *old_root = tree->root;
	struct rnode *new_root;
	struct llist *swap_list;
	struct rnode *node;
	struct list_elem *elem;

	/* Insert node (will be the new root) above outgroup */
	if (! insert_node_above(outgroup, ""))
		return FAILURE;
	new_root = outgroup->parent;

	/* We need to swap the nodes from new root to the old root (i.e., swap
	 * a node with its parent), so the tree is always in a consistent
	 * state */

	/* First, we make a list of the nodes we need to swap, by visiting
	 * the tree from the soon-to-be new root to the old (which is still the
	 * root) */
	swap_list = create_llist();
	if (NULL == swap_list) return FAILURE;
	for (node = new_root; ! is_root(node); node = node->parent) {
		/* order of swaps is important: tree is always consistent */
		if (! prepend_element(swap_list, node)) return FAILURE;
	}
	/* Now, we swap the nodes in the list. */ 
	for (elem = swap_list->head; NULL != elem; elem = elem->next) {
		struct rnode *to_swap = elem->data;
		if (! swap_nodes(to_swap)) return FAILURE;
	}
        destroy_llist(swap_list);

	if (children_count(old_root) == 1) {
		if (! splice_out_rnode(old_root))
			return FAILURE;
	}

	tree->root = new_root;
        destroy_llist(tree->nodes_in_order);
	tree->nodes_in_order = get_nodes_in_order(tree->root);

	return SUCCESS;
}

void collapse_pure_clades(struct rooted_tree *tree)
{
	struct list_elem *el;		

	for (el = tree->nodes_in_order->head; NULL != el; el = el->next) {
		struct rnode *current = el->data;
		if (is_leaf(current)) continue;	/* can only collapse inner nodes */
		/* attempt collapse only if all children are leaves (any pure
		 * subtree will have been collapsed to a leaf by now) */
		if (! all_children_are_leaves(current)) continue;
		char *label;
		if (all_children_have_same_label(current, &label)) {
			/* set own label to children's label  - we copy it
			 * because it will be later passed to free() */
			free(current->label);
			current->label = strdup(label);
			remove_children(current);
		}
	}
}

void destroy_tree(struct rooted_tree *tree)
{
	struct list_elem *e;

	/* The nodes themselves are destroyed using destroy_all_rnodes() */

	destroy_llist(tree->nodes_in_order);
	free(tree);
}

int leaf_count(struct rooted_tree * tree)
{
	struct list_elem *el;
	int n = 0;

	for (el = tree->nodes_in_order->head; NULL != el; el = el->next) {
		if (is_leaf((struct rnode *) el->data)) {
			n++;
		}
	}

	return n;
}

/* NOTE: this f() is not used in app code, but at least 1 test does use it (
 test_graph_common.c) */

struct llist *get_leaf_labels(struct rooted_tree *tree)
{
	struct llist *labels = create_llist();
	if (NULL == labels) return NULL;
	struct list_elem *el;

	for (el = tree->nodes_in_order->head; NULL != el; el = el->next) {
		struct rnode *current = (struct rnode *) el->data;
		if (is_leaf(current)) 
			if (strcmp ("", current->label) != 0)
				if (! append_element(labels, current->label))
					return NULL;
	}

	return labels;
}

bool is_cladogram(struct rooted_tree *tree)
{
	return TREE_TYPE_CLADOGRAM == get_tree_type(tree);
}

/* This could be derived by the parser and stored in a member of the
 * rooted_tree structure, but since not all apps need to know whether or not a
 * tree is a cladogram, we decided that the parser should concentrate on
 * building the data structure. We use a 'lazy' approach here: the type is
 * determined once (on demand), and remembered afterwards. */

enum tree_type get_tree_type(struct rooted_tree *tree)
{
	if (TREE_TYPE_UNKNOWN != tree->type)
		return tree->type;
	
	/* Type is not known, so we compute it. */
	int nb_nodes = tree->nodes_in_order->count;
	int nb_edges_with_lengths = 0;
	int nb_edges_without_lengths = 0;
	struct list_elem *el;
	for (el = tree->nodes_in_order->head; NULL != el; el = el->next) {
		struct rnode *current = el->data;
		if (strcmp(current->edge_length_as_string, "") == 0)	/* length is empty (NOT zero!) */
			nb_edges_without_lengths++;
		else
			nb_edges_with_lengths++;
	}
	
	if (nb_edges_without_lengths == nb_nodes) 
		return TREE_TYPE_CLADOGRAM;
	else if (nb_edges_with_lengths == nb_nodes) 
		return TREE_TYPE_PHYLOGRAM;
	else if (nb_edges_with_lengths == nb_nodes - 1
		&& strcmp("", tree->root->edge_length_as_string) == 0)
		return TREE_TYPE_PHYLOGRAM;
	else
		return TREE_TYPE_NEITHER;	/* weird, but legal */
}

struct llist *nodes_from_labels(struct rooted_tree *tree,
		struct llist *labels)
{
	struct hash *label2node_map = create_label2node_map(
			tree->nodes_in_order); 
	struct llist *result = create_llist();
	if (NULL == result) return NULL;
	struct list_elem *el;
	for (el = labels->head; NULL != el; el = el->next) {
		char *label = el->data;
		struct rnode *node = hash_get(label2node_map, label);
		if (NULL == node) 
			fprintf (stderr, "WARNING: label '%s' not found.\n",
					label);
		else 
			if (! append_element(result, node))
				return NULL;
	}
	destroy_hash(label2node_map);

	return result;
}


struct llist *nodes_from_regexp(struct rooted_tree *tree, regex_t *preg)
{
       				       
	int errcode;
	struct llist *result = create_llist();
	if (NULL == result) return NULL;
	struct list_elem *el;

	size_t nmatch = 1;	/* either matches or doesn't */
	regmatch_t pmatch[nmatch]; 
	int eflags = 0;

	for (el = tree->nodes_in_order->head; NULL != el; el = el->next) {
		struct rnode *node = el->data;
		errcode = regexec(preg, node->label, nmatch, pmatch, eflags);	
		if (0 == errcode) {
			if (! append_element(result, node))
				return NULL;
		}
	}

	return result;
}
