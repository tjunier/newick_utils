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
#include <string.h>
#include <stdio.h>
#include <assert.h>

#include "rnode.h"
#include "rnode_iterator.h"
#include "hash.h"
#include "common.h"
#include "list.h"

struct rnode *create_rnode(char *label, char *length_as_string)
{
	struct rnode *node;

	node = malloc(sizeof(struct rnode));
	if (NULL == node) return NULL;

	if (NULL == label) {
		label = "";
	}
	if (NULL == length_as_string) {
		length_as_string = "";
	}
	node->label = strdup(label);
	node->edge_length_as_string = strdup(length_as_string);
	node->parent = NULL;
	node->next_sibling = NULL;
	node->first_child = NULL;
	node->child_count = 0;
	node->data = NULL;
	/* We must initialize to some numeric value, so we use -1 to mean
	 * unknown/undetermined. Note that negative branch lengths can occur,
	 * e.g. with neighbor-joining. The reference variable here is
	 * length_as_string, which will be an empty string ("") if the length
	 * is not defined (as in cladograms). */
	node->edge_length = -1;	// TODO: check that this is used.
	node->current_child = NULL;
	node->seen = 0;

#ifdef SHOW_RNODE_CREATE
	fprintf(stderr, "creating rnode %p '%s'\n", node, node->label);
#endif

	return node;
}

void destroy_rnode(struct rnode *node, void (*free_data)(void *))
{
#ifdef SHOW_RNODE_DESTROY
	fprintf (stderr, " freeing rnode %p '%s'\n", node, node->label);
#endif
	free(node->label);
	free(node->edge_length_as_string);
	/* if free_data is not NULL, we call it to free the node data (use this
	 * when the data cannot just be free()d); otherwise we just free()
	 * node->data  */
	if (NULL != free_data)
		free_data(node->data);
	else if (NULL != node->data)
		free(node->data);
	free(node);
}

inline int children_count(struct rnode *node)
{
	return node->child_count;
}

int is_leaf(struct rnode *node)
{
	return 0 == node->child_count;
}

int is_root(struct rnode *node)
{
	return (NULL == node->parent);
}

int is_inner_node(struct rnode *node)
{
	return 	(!is_leaf(node) && !is_root(node));
}

bool all_children_are_leaves(struct rnode *node)
{
	if (is_leaf(node))
		return false;

	struct rnode *curr;
	for (curr=node->first_child; NULL != curr; curr=curr->next_sibling)
		if (! is_leaf(curr)) return false;

	return true;
}

bool all_children_have_same_label(struct rnode *node, char **label)
{

	if (is_leaf(node))
		return false;

	/* get first child's label */
	struct rnode *curr = node->first_child;
	char *ref_label = curr->label;

	/* iterate over other children, and compare their label to the first's
	 * */

	*label = NULL;
	for (curr = curr->next_sibling; NULL != curr; curr = curr->next_sibling)
		if (0 != strcmp(ref_label, curr->label))
			return 0; /* found a different label */

	*label = ref_label;
	return 1;
}

void dump_rnode(void *arg)
{
	struct rnode *node = (struct rnode *) arg;

	printf ("\nrnode at %p\n", node);
	printf ("  label at %p = '%s'\n", node->label, node->label);
	printf ("  edge length = '%s'\n", node->edge_length_as_string);
	printf ("              = %f\n", node->edge_length);
	printf ("  1st child   = %p\n", node->first_child);
	printf ("  data    = %p\n", node->data);
}

/* If something fails, we just return. */
// TODO: consider just computing the nodes_in_order list and free()ing as
// usual; if not possible then add as an example (in rnode_iterator.[ch]) of
// when this is not possible
void free_descendants(struct rnode *node)
{
	const int HASH_SIZE = 1000; 	/* pretty arbitrary */
	struct hash *to_free = create_hash(HASH_SIZE);
	if (NULL == to_free) return;	
	struct rnode_iterator *it = create_rnode_iterator(node);
	if (NULL == it) return;
	struct rnode *current;

	/* Iterates through the tree nodes, "remembering" nodes seen for the
	 * first time */
	while (NULL != (current = rnode_iterator_next(it))) {
		char *node_hash_key = make_hash_key(current);
		if (NULL == hash_get(to_free, node_hash_key))
			if (! hash_set(to_free, node_hash_key, current))
				return; 
                free(node_hash_key);
	}

	// Frees all nodes "seen" above - which must be all the descendants of
	// 'node'.
	struct llist *keys = hash_keys(to_free);
	if (NULL == keys) return;
       	struct list_elem *el;
	for (el = keys->head; NULL != el; el = el->next) {
		char *key = el->data;
		current = hash_get(to_free, key);
		destroy_rnode(current, NULL);
	}	

        destroy_llist(keys);
        destroy_hash(to_free);
	destroy_rnode_iterator(it);
}

struct rnode** children_array(struct rnode *node)
{
	struct rnode **array = malloc(node->child_count *
			sizeof(struct rnode *));
	if (NULL == array) return NULL;
	memset(array, '\0', node->child_count);

	struct rnode *kid = NULL;
	int i = 0;
	for (kid = node->first_child; NULL != kid; kid = kid->next_sibling) {
		printf("%d/%d\n", i, node->child_count);
		array[i] = kid;
		i++;
	}

	return array;
}
