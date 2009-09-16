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

#define SEEN "SEEN"

static const int INIT_HASH_SIZE = 1000;

struct rnode_iterator *create_rnode_iterator(struct rnode *root)
{
	struct rnode_iterator *iter;
	iter = (struct rnode_iterator *) malloc(sizeof(struct rnode_iterator));
	if (NULL == iter) return NULL;

	iter->root = iter->current = root;
	iter->seen = create_hash(INIT_HASH_SIZE);
	if (NULL == iter->seen) return NULL;
	iter->status = RNODE_ITERATOR_INIT;

	return iter;
}

void destroy_rnode_iterator (struct rnode_iterator *it)
{
	destroy_hash(it->seen);
	free(it);
}

/* Returns the current node's next un-visited child, or NULL if there is none.
 * Will return NULL if node has no children. */

/* TODO: currently, this function "remembers" visited nodes (in the iter->seen
 * hash). At every call, it traverses the current node's children list until if
 * finds at child it has not seen. This takes O(n) time, for n children. Since
 * this function will be called once per child, the result is O(n²). This could
 * be improved by "remembering" the last list_elem visited in the children
 * list. Since this has a pointer to the next element, getting the next
 * unvisited child takes constant time. There just needs to be a stack of
 * "remembered" list_elem structures in struct rnode_iterator (instead of the
 * hash, which has the added benefit of using up less space). When going down
 * to a child, the current list_elem is pushed on the stack, and it is popped
 * out of it when going back up. */

/* Note however that this will only make a difference for trees that are
 * polytomous. For dichotomous trees, the present approach is probably the
 * best. */

static struct rnode * get_next_unvisited_child(struct rnode_iterator *iter)
{
	struct list_elem *elem;

	for (elem = iter->current->children->head; NULL != elem;
			elem = elem->next) {
		struct rnode *child = elem->data;
		char *node_hash_key = make_hash_key(child);
		if (NULL == hash_get(iter->seen, node_hash_key)) {
			free(node_hash_key);
			return child;
		}
		free(node_hash_key);
	}
	return NULL;	/* no unvisited child left */
}

// caller checked
struct rnode *rnode_iterator_next(struct rnode_iterator *iter)
{
	char *current_node_hash_key = make_hash_key(iter->current);

	/* We have to consider the case of a single-node tree (this happens
	 * e.g. in nw_match if none of the labels in the pattern tree is found
	 * in the target tree). The single node is in this case both a leaf (no
	 * children) and the root (no parent). Hence the double test below. */
	if (is_leaf(iter->current) && ! is_root(iter->current)) {
		if (! hash_set(iter->seen, current_node_hash_key, SEEN)) {
			iter->status = RNODE_ITERATOR_ERROR;
			return NULL;
		}
		iter->current = iter->current->parent;
		free(current_node_hash_key);
		return iter->current;
	} else {
		struct rnode *next_child = get_next_unvisited_child(iter);
		if (NULL != next_child) {
			/* proceed to next child */
			iter->current = next_child;
			free(current_node_hash_key);
			return iter->current;
		} else {
			if (! hash_set(iter->seen, current_node_hash_key,
						SEEN)) {
				iter->status = RNODE_ITERATOR_ERROR;
				return NULL;
			}
			free(current_node_hash_key);
			if (iter->current == iter->root) {
				iter->status = RNODE_ITERATOR_END;
				return NULL;
			} else {
				iter->current = iter->current->parent;
				return iter->current;
			}
		}
	}
}

/* Computes the list by doing a tree traversal, then reversing it, printing out
 * each node the first time it sees it. */

// caller checked
struct llist *get_nodes_in_order(struct rnode *root)
{
	struct rnode_iterator *it = create_rnode_iterator(root);
	if (NULL == it) return NULL;
	struct hash *seen = create_hash(INIT_HASH_SIZE);
	if (NULL == seen) return NULL;
	struct rnode *current;
	struct llist *traversal = create_llist();
	if (NULL == traversal) return NULL;
	struct llist *reverse_traversal;
	struct llist *nodes_in_reverse_order = create_llist();
	if (NULL == nodes_in_reverse_order) return NULL;
	struct llist *nodes_in_order;
	char *current_hash_key;

	/* Iterates over the whole tree - note that a node is visited more than
	 * once, except leaves. */
	while ((current = rnode_iterator_next(it)) != NULL) {
		if (! append_element (traversal, current)) return NULL;
	}
	/* rnode_iterator_next() returned NULL: see why */
	switch (it->status) {
		case RNODE_ITERATOR_END:
			break;	/* Ok */
		case RNODE_ITERATOR_ERROR:
			return NULL;
		case RNODE_ITERATOR_INIT: /* should now be end or error */
		default:
			assert(0);	/* programmer error */
	}

	destroy_rnode_iterator(it);

	reverse_traversal = llist_reverse(traversal);
	if (NULL == reverse_traversal) return NULL;
	destroy_llist(traversal);

	/* This keeps only the first 'visit' through any node */
	// TODO: can't this be done in the first traversal, above?
	struct list_elem *el;
	for (el = reverse_traversal->head; NULL != el; el = el->next) {
		current = el->data;
		current_hash_key = make_hash_key(current);
		if (NULL == hash_get(seen, current_hash_key)) {
			/* Not seen yet? add to list, and mark as seen (hash) */
			if (! append_element
					(nodes_in_reverse_order, current))
				return NULL;
			/* Could use anything - existential hash */
			if (! hash_set(seen, current_hash_key, current))
				return NULL;
		}
		free(current_hash_key);
	}

	destroy_llist(reverse_traversal);
	nodes_in_order = llist_reverse(nodes_in_reverse_order);
	if (NULL == nodes_in_order) return NULL;
	destroy_llist(nodes_in_reverse_order);
	destroy_hash(seen);

	return nodes_in_order;
}

// caller checked
struct hash *get_leaf_label_map(struct rnode *root)
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
	/* See why rnode_iterator_next() returned NULL */
	switch (it->status) {
		case RNODE_ITERATOR_END:
			break;	/* Ok */
		case RNODE_ITERATOR_ERROR:
			return NULL;
		default:
			assert(0);	/* programmer error */
	}

	destroy_rnode_iterator(it);

	return result;
}
