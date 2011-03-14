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

#include "list.h"
#include "rnode.h"
#include "hash.h"
#include "common.h"
#include "rnode_iterator.h"

struct hash * create_label2node_map(struct llist *node_list)
{
	struct hash *map;
	struct list_elem *elem;
	
	map = create_hash(node_list->count);
	if (NULL == map) return NULL;

	for (elem = node_list->head; NULL != elem; elem = elem->next) {
		struct rnode *current = (struct rnode *) elem->data;
		if (strcmp("", current->label) == 0) { continue; }
		if (! hash_set(map, current->label, current)) return NULL;
	}

	return map;
}

struct hash * create_label2node_list_map(struct llist *node_list)
{
	/* At most there will be one hash element per list element, so this
	 * will be enough. */
	struct hash *map = create_hash(node_list->count);	
	if (NULL == map) return NULL;

	struct list_elem *elem;

	for (elem = node_list->head; NULL != elem; elem = elem->next) {
		struct rnode *current = elem->data;
		char *current_lbl = current->label;
		/* See if we have already seen this label: if so there is a
		 * list for it in 'map' */
		struct llist *lbl_node_list = hash_get(map, current_lbl);
		if (NULL == lbl_node_list) {
			/* Create the list and add * it to the hash */
		 	lbl_node_list = create_llist();
			if (NULL == lbl_node_list) return NULL;
			if (! hash_set(map, current_lbl, lbl_node_list))
				return NULL;
		}
		/* Now add the current node to the list */
		if (! append_element (lbl_node_list, current))
			return NULL;
	}

	return map;
}

/* NOTE: we can't just destroy this hash, because its elements are lists and
 * most therefore be destroyed with destroy_llist() rather than just free(). */

void destroy_label2node_list_map(struct hash *map)
{

	struct llist *keys = hash_keys(map);	
	if (NULL == keys) return;
	struct list_elem *el;
	for (el = keys->head; NULL != el; el = el->next) {
		char *label = el->data;
		struct llist *nodes_list = hash_get(map, label);
		destroy_llist(nodes_list);
	}
	destroy_llist(keys);
	destroy_hash(map);
}

/* Returns a label->node map of (labeled) leaves descending from 'root' */
/* Nodes' 'seen' member must be zero - see note above about the 'seen' member
 * of struct rnode. NOTE: this function is meant to work on any node, not just
 * a tree's root (e.g. in is_monophyletic()). So don't be tempted to rewrite it
 * to use tree->nodes_in_order. */

static const int INIT_HASH_SIZE = 1000;

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

