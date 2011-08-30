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

/* These variables are for keeping track of all allocated rnodes, so that we
 * can free them all (one call to free them all :-) */
/* NOTE: since all rnode pointers are stored in the rnode_array, the memory
 * they occupy will never count as a leak unless and until rnode_array is
 * free()d. This happens in destroy_all_rnodes(), so don't forget to call it.
 * */

static const int rnode_array_size_increment = 1000;
static int rnode_count = 0;
static int rnode_array_size = 0;	/* in number of nodes */
static struct rnode** rnode_array = NULL;

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
	node->last_child = NULL;
	node->child_count = 0;
	node->data = NULL;
	/* We must initialize to some numeric value, so we use -1 to mean
	 * unknown/undetermined. Note that negative branch lengths can occur,
	 * e.g. with neighbor-joining. The reference variable here is
	 * length_as_string, which will be an empty string ("") if the length
	 * is not defined (as in cladograms). */
	node->edge_length = -1;	
	node->current_child = NULL;
	node->seen = 0;

#ifdef SHOW_RNODE_CREATE
	fprintf(stderr, "creating rnode %p '%s'\n", node, node->label);
#endif

	/* Now add to list of nodes */
	rnode_count++;
	if (rnode_count > rnode_array_size) {
		int new_size = rnode_array_size + rnode_array_size_increment;
		rnode_array = realloc(rnode_array,
			new_size * sizeof(struct rnode*));
		if (NULL == rnode_array) return NULL;
		rnode_array_size = new_size;
	}
	rnode_array[rnode_count - 1] = node;
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


void destroy_all_rnodes(void (*free_data)(void *))
{
	while (rnode_count > 0)
		destroy_rnode(rnode_array[--rnode_count], free_data);
	free(rnode_array);
	rnode_array_size = 0;
	rnode_array = NULL;
}

void show_all_rnodes()
{
	struct rnode **rnode_h;
	for (rnode_h = rnode_array; NULL != *rnode_h; rnode_h++) {
		dump_rnode(*rnode_h);
	}
}

inline int children_count(struct rnode *node)
{
	return node->child_count;
}

bool is_leaf(struct rnode *node)
{
	return 0 == node->child_count;
}

bool is_root(struct rnode *node)
{
	return (NULL == node->parent);
}

bool is_inner_node(struct rnode *node)
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

struct rnode** children_array(struct rnode *node)
{
	struct rnode **array = malloc(node->child_count *
			sizeof(struct rnode *));
	if (NULL == array) return NULL;
	memset(array, '\0', node->child_count);

	struct rnode *kid = NULL;
	int i = 0;
	for (kid = node->first_child; NULL != kid; kid = kid->next_sibling) {
		array[i] = kid;
		i++;
	}

	return array;
}

/* Computes the list by doing a tree traversal, then reversing it. */

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

int _get_rnode_count() { return rnode_count; }
