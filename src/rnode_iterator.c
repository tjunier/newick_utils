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
#include <assert.h>

#include "rnode_iterator.h"
#include "hash.h"
#include "list.h"
#include "rnode.h"
#include "tree.h"

/* The functions in this module provide an iterator interface on a node and its
 * descendents, allowing traversal. The low-level rnode_iterator and functions
 * that call it directly (such as rnode_iterator_next()) visit the tree by
 * following edges (depth first, and visiting each child node in order). All
 * nodes except leaves are thus visited more than once. Higher-level functions
 * (like get_nodes_in_order()) can discard already-visited nodes and produce
 * e.g. post-order traversals, etc. */

/* In general, there is no need to use these functions because most operations
 * can be done using a tree's 'nodes_in_order' list. Looping on this list will
 * be faster than iterating on a subtree. But there are exceptions:

 o the 'nodes_in_order' list may be outdated (e.g. because nodes were inserted,
   deleted, etc) - in that case, the list should be reconstructed with
   get_nodes_in_order(), which uses the rnode iterator.
 
 o the 'nodes_in_order' list may not contain all the needed information (this
   is the case when outputting Newick).

 */

/* NOTE: functions in this module now rely on the 'seen' member of struct
 * rnode. The former approach, namely to 'remember' visited nodes with hash
 * tables, proved too slow. It is implicitly assumed that the 'seen' member of
 * each node in the tree is zero before the functions are called. You can use
 * the convenience function reset_seen() for this. */

/* Another possible imlementation would be to use a stack of nodes to visit,
 * and processing the top node until the statck is empty. Processing would mean
 * pushing all the node's children, then removing the node. But I'm not sure it
 * would be faster than using 'seen' flags. */

static const int INIT_HASH_SIZE = 1000;

/* see note above about the 'seen' member of struct rnode */

struct rnode_iterator *create_rnode_iterator(struct rnode *root)
{
	struct rnode_iterator *iter;
	iter = (struct rnode_iterator *) malloc(sizeof(struct rnode_iterator));
	if (NULL == iter) return NULL;

	iter->root = iter->current = root;

	return iter;
}

void destroy_rnode_iterator (struct rnode_iterator *it)
{
	free(it);
}

/* Returns true IFF there are more children to visit on the current node. */

bool more_children_to_visit (struct rnode_iterator *iter)
{
	if  (iter->current->current_child_elem
	    != iter->current->children->tail) 
		return true;
	else
		return false;
}

struct rnode *rnode_iterator_next(struct rnode_iterator *iter)
{
	/* We have to consider the case of a single-node tree (this happens
	 * e.g. in nw_match if none of the labels in the pattern tree is found
	 * in the target tree). The single node is in this case both a leaf (no
	 * children) and the root (no parent). Hence the double test below. */
	if (is_leaf(iter->current) && ! is_root(iter->current)) {
		iter->current = iter->current->parent;
		return iter->current;
	}

	// TODO: this case may in fact be handled by the next one.
	if (is_leaf(iter->current)) {
		iter->current = iter->current->parent;
		return iter->current;
	}

	if (iter->current->current_child_elem
	    == iter->current->children->tail) {
		if (iter->root == iter->current) {
			return NULL;
		} else {
			iter->current = iter->current->parent;
			return iter->current;
		}
	}

	if (NULL == iter->current->current_child_elem) 
		iter->current->current_child_elem =
			iter->current->children->head;
	else
		iter->current->current_child_elem =
			iter->current->current_child_elem->next;

	struct rnode *next = iter->current->current_child_elem->data;
	iter->current = next;
	return next;
}

/* Computes the list by doing a tree traversal, then reversing it, printing out
 * each node the first time it sees it. */
/* see note above about the 'seen' member of struct rnode */

struct llist *get_nodes_in_order(struct rnode *root)
{
	struct rnode_iterator *it = create_rnode_iterator(root);
	if (NULL == it) return NULL;
	struct rnode *current;
	struct llist *traversal = create_llist();
	if (NULL == traversal) return NULL;
	struct llist *reverse_traversal;
	struct llist *nodes_in_reverse_order = create_llist();
	if (NULL == nodes_in_reverse_order) return NULL;
	struct llist *nodes_in_order;

	/* Iterates over the whole tree - note that a node is visited more than
	 * once, except leaves. */
	while ((current = rnode_iterator_next(it)) != NULL) {
		current->seen = 0;
		if (! append_element (traversal, current)) return NULL;
	}

	destroy_rnode_iterator(it);

	reverse_traversal = llist_reverse(traversal);
	if (NULL == reverse_traversal) return NULL;
	destroy_llist(traversal);

	/* This keeps only the first 'visit' through any node */
	struct list_elem *el;
	for (el = reverse_traversal->head; NULL != el; el = el->next) {
		current = el->data;
		/* Nodes will have been seen by the iterator above, hence they
		 * start with a 'seen' value of 1. */
		if (current->seen == 0) {
			/* Not seen yet? add to list, and mark as seen (hash) */
			if (! append_element
					(nodes_in_reverse_order, current))
				return NULL;
			current->seen = 1;
		}
	}

	destroy_llist(reverse_traversal);
	nodes_in_order = llist_reverse(nodes_in_reverse_order);
	if (NULL == nodes_in_order) return NULL;
	destroy_llist(nodes_in_reverse_order);

	/* remove the 'seen' marks */
	for (el = nodes_in_order->head; NULL != el; el = el->next) {
		current = el->data;
		current->seen = 0;
	}
	return nodes_in_order;
}

/* Returns a label->node map of (labeled) leaves */
/* Nodes' 'seen' member must be zero - see note above about the 'seen' member of struct rnode */
// TODO: this function should use the tree's nodes_in_order list. If invalid or
// NULL, it should be computed by calling get_nodes_in_order() on the root. 

struct hash *get_leaf_label_map_from_node(struct rnode *root)
{
	struct rnode_iterator *it = create_rnode_iterator(root);
	if (NULL == it) return NULL;
	struct rnode *current;
	struct hash *result = create_hash(INIT_HASH_SIZE);
	if (NULL == result) return NULL;

	while ((current = rnode_iterator_next(it)) != NULL) {
		if (is_leaf(current)) {
			if (strcmp("", current->label) != 0) {
				if (! hash_set(result,
					current->label, current)) 
						return NULL;
			}
		}
	}

	destroy_rnode_iterator(it);

	return result;
}

void reset_current_child_elem(struct rooted_tree *tree)
{
	struct list_elem *el;

	for (el = tree->nodes_in_order->head; NULL != el; el = el->next) {
		struct rnode *current = el->data;
		current->current_child_elem = NULL;
	}
}
