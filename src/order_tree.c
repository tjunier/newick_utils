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
/* order_tree.c - do not confuse with order.c, which is the ordering program.
 * This is just the implementation of functions used by it (and others, which
 * is why they are now separated) */

#include <string.h>
#include <stdlib.h>

#include "rnode.h"
#include "tree.h"
#include "list.h"
#include "link.h"
#include "common.h"
#include "order_tree.h"

int order_tree(struct rooted_tree *tree,
		int (*comparator)(const void*,const void*),
		int (*sort_field_setter)(struct rnode *))
{
	struct list_elem *elem;

	/* the rnode->data member is used to store the sort field. This is set
	 * by the set_sort_field_num_desc callback.*/

	for (elem=tree->nodes_in_order->head; NULL!=elem; elem=elem->next) {
		struct rnode *current = elem->data;
		if (is_leaf(current)) {
			sort_field_setter(current);
		} else {
			/* Since all children have been visited (because we're
			 * traversing the tree in postorder), we can
			 * just order the children on their sort field. */
			struct rnode ** kids_array;
			int count = current->child_count;
			kids_array = (struct rnode **)
				children_array(current);
			if (NULL == kids_array) return FAILURE;

			qsort(kids_array, count, sizeof(struct rnode *),
					comparator);

			remove_children(current);
			int i;
			for (i = 0; i < count; i++)
				add_child(current, kids_array[i]);
			current->last_child->next_sibling = NULL;

			sort_field_setter(current);
			free(kids_array);
		}
	}

	return SUCCESS;
}

int order_tree_lbl(struct rooted_tree *tree)
{
	return order_tree(tree, lbl_comparator, set_sort_field_label);
}

int num_desc_comparator(const void *a, const void *b)
{
	struct rnode *rnode_a = *((struct rnode **) a);
	struct rnode *rnode_b = *((struct rnode **) b);
	int a_num_desc = *((int *) rnode_a->data);
	int b_num_desc = *((int *) rnode_b->data);

	if (a_num_desc > b_num_desc)
		return 1;
	if (a_num_desc < b_num_desc)
		return -1;
	return 0;
}

int num_desc_deladderize(const void *a, const void *b)
{
	static int orientation = -1;

	/* This changes sign at every invocation, therefore one call considers
	 * a heavy node greater than a light node (in terms of number of
	 * descendants), but the next call does the opposite. */
	orientation *= -1;

	struct rnode *rnode_a = *((struct rnode **) a);
	struct rnode *rnode_b = *((struct rnode **) b);
	int a_num_desc = *((int *) rnode_a->data);
	int b_num_desc = *((int *) rnode_b->data);

	if (a_num_desc > b_num_desc)
		return orientation;
	if (a_num_desc < b_num_desc)
		return -orientation;
	return 0;
}

int lbl_comparator(const void *a, const void *b)
{
	/* I really have trouble understanding how qsort() passes the
	 * comparands to the comparator... but thanks to GDB I figured out this
	 * one. */	
	char *a_lbl = (*(struct rnode **)a)->data;
	char *b_lbl = (*(struct rnode **)b)->data;

	int cmp = strcmp(a_lbl, b_lbl);
	// printf ("%s <=> %s: %d\n", a_lbl, b_lbl, cmp);

	return cmp;
}

int order_tree_num_desc(struct rooted_tree *tree)
{
	return order_tree(tree, num_desc_comparator, set_sort_field_num_desc);
}

int order_tree_deladderize(struct rooted_tree *tree)
{
	return order_tree(tree, num_desc_deladderize, set_sort_field_num_desc);
}

int set_sort_field_label(struct rnode *node)
{
	/* If the node has a non-empty label, or if it is a leaf (or both), we
	 * just use the label as sort field. Otherwise, we use the first
	 * child's sort field.  */

	if (is_leaf(node) || (strcmp(node->label, "") != 0))
		node->data = strdup(node->label);
	else 
		node->data = strdup((char *) node->first_child->data);

	if (NULL == node->data)
		return FAILURE;

	return SUCCESS;
}

int set_sort_field_num_desc(struct rnode *node)
{

	/* node's descendant count is 1 + the sum of the descendant counts of
	 * its children */

	node->data = malloc(sizeof(int));
	if (NULL == node->data) return FAILURE;

	if (is_leaf(node)) {
		*((int *) node->data) = 1;	/* counts as one */
	} else {
		int sum = 0;
		struct rnode *kid = node->first_child;
		for(; NULL != kid; kid = kid->next_sibling)
			sum += *((int *) kid->data);
		*((int *) node->data) = sum;
	}

	return SUCCESS;
}
