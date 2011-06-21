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

#include "tree.h"
#include "rnode.h"
#include "list.h"
#include "hash.h"
#include "nodemap.h"
#include "error.h"

struct rooted_tree *lca2w_tree;

/* NOTE: these two functions are obsolete, but I keep them in case the new
 * implementation turns out to be faulty. */

/* marks all nodes of the tree unseen */

/*
void mark_unseen(struct rooted_tree *tree)
{
	struct list_elem *el;

	for (el=tree->nodes_in_order->head; NULL!=el; el=el->next) {
		struct rnode *current = (struct rnode *) el->data;
		current->data = NULL;
	}
}
*/

/* Start from descendant A and climbs to the root, marking each node as 'seen'
 * along the way. Then starts again from descendant B, returning the first
 * 'seen' node. */

struct rnode *lca2(struct rooted_tree *tree, struct rnode *desc_A,
		struct rnode *desc_B)
{
	char *SEEN = "seen";
	char *key;

	/* This hash will remember which nodes have been visited. It need be at
	 * most as large as the number of nodes in the tree. */
	struct hash *seen_nodes = create_hash(tree->nodes_in_order->count);
	if (NULL == seen_nodes) return NULL;

	/* Climb to root, marking nodes as 'seen' */
	while (! is_root(desc_A)) {
		key = make_hash_key(desc_A);
		if(! hash_set(seen_nodes, key, SEEN)) return NULL;
		free(key);
		desc_A = desc_A->parent;
	}
	key = make_hash_key(desc_A);
	if (! hash_set(seen_nodes, key, SEEN)) return NULL;
	free(key);

	while (1) {
		key = make_hash_key(desc_B);
		if (NULL != hash_get(seen_nodes, key)) { /* seen */
			free(key);
			break;
		}
		free(key);
		desc_B = desc_B->parent;
	}

	destroy_hash(seen_nodes);

	return desc_B;
}

/* A wrapper around lca2() so it can be passed to reduce() */

void * lca2w(void *a, void *b)
{
	return lca2(lca2w_tree, (struct rnode*) a,
			(struct rnode *) b);
}

/* Returns the LCA of any number of nodes, by applying lca2 to a list of
 * descendants. THIS USES UP THE LIST! Use lca_from_nodes() to keep the
 * argument list intact. */

static struct rnode *lca (struct rooted_tree *tree,
		struct llist *descendants)
{
	void * result;

	/* lca2w() has only 2 args (by definition of reduce() in
	 * list.h), but lca() needs *tree */
	lca2w_tree = tree; 
	result = reduce(descendants, lca2w);

	return result;
}

struct rnode *lca_from_nodes (struct rooted_tree *tree,
		struct llist *descendants)
{
	struct rnode *result;
	struct llist *copy = shallow_copy(descendants);
	if (NULL == copy) return NULL;

	result = lca(tree, copy);
	if (NULL == result) return NULL;

	destroy_llist(copy);
	return result;
}

struct rnode *lca_from_labels(struct rooted_tree *tree, struct llist *labels)
{
	struct hash *node_map = create_label2node_map(tree->nodes_in_order);
	if (NULL == node_map) return NULL;
	struct llist *descendant_nodes = create_llist();
	if (NULL == descendant_nodes) return NULL;
	struct list_elem *el;

	for (el = labels->head; NULL != el; el = el->next) {
		char *label = (char *) el->data;
		if (0 == strcmp("", label)) {
			fprintf(stderr, "WARNING: empty label.\n");
			continue;
		}
		struct rnode *desc = hash_get(node_map, label);
		if (NULL == desc) {
			fprintf(stderr, "WARNING: no node with label '%s'.\n",
					label);
			continue;
		}
		if (! append_element(descendant_nodes, desc))
			return NULL;
	}

	struct rnode *result = lca(tree, descendant_nodes);
	if (NULL == result) return NULL;

	destroy_hash(node_map);
	destroy_llist(descendant_nodes);

	return result;
}

struct rnode *lca_from_labels_multi (struct rooted_tree *tree, 
		struct llist *labels)
{
	/* If something happens, it's most likely to be a memory problem: */
	set_last_error_code(ERR_NOMEM);

	/* Make a hash of lists of nodes of the same label */
	struct hash *nodes_by_label;
       	nodes_by_label = create_label2node_list_map(tree->nodes_in_order);
	if (NULL == nodes_by_label) return NULL;

	/* Iterate over labels, add all nodes that have the current label
	 * (there may be more than one) to the list of descendants. */

	struct llist *descendants = create_llist();
	if (NULL == descendants) return NULL;
	struct list_elem *elem;
	for (elem = labels->head; NULL != elem; elem = elem->next) {
		char *label = elem->data;
		struct llist *nodes_list = hash_get(nodes_by_label, label);
		if (NULL == nodes_list)
			fprintf (stderr, "WARNING: label '%s' not found.\n",
					label);
		else  {
			struct llist *copy = shallow_copy(nodes_list);
			if (NULL == copy) return NULL;
			append_list (descendants, copy);
			free(copy); 	/* NOT destroy_llist(): the list
					   elements are in descendants. */
		}
	}

	/* No nodes were found that matched the labels */
	if (0 == descendants->count) {
		set_last_error_code(ERR_NO_MATCHING_NODES);
		destroy_llist(descendants);
		destroy_label2node_list_map(nodes_by_label);
		return NULL;
	}

	struct rnode *result = lca_from_nodes (tree, descendants);
	if (NULL == result) return NULL;

	destroy_llist(descendants);
	destroy_label2node_list_map(nodes_by_label);

	return result;
}

