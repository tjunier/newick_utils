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
#include "common.h"
#include "order_tree.h"

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

int order_tree(struct rooted_tree *tree)
{
	struct list_elem *elem;

	/* the rnode->data member is used to store the sort field. For leaves,
	 * this is just the label; for inner nodes it is the sort field of the
	 * first child (after sorting). */

	for (elem=tree->nodes_in_order->head; NULL!=elem; elem=elem->next) {
		struct rnode *current = elem->data;
		if (is_leaf(current)) {
			current->data = current->label;
		} else {
			/* Since all children have been visited (because we're
			 * traversing the tree in parse order), we can just
			 * order the children on their sort field. */

			struct rnode ** kids_array;
			int count = current->children->count;
			kids_array = (struct rnode **)
				llist_to_array(current->children);
			if (NULL == kids_array) return FAILURE;
			destroy_llist(current->children);
			qsort(kids_array, count, sizeof(struct redge *),
					lbl_comparator);
			struct llist *ordered_kids_list;
			ordered_kids_list = array_to_llist(
				(void **) kids_array, count);
			current->children = ordered_kids_list;

			// Get sort field from first child ("back-inherit") [?]
			current->data = kids_array[0]->data;
			free(kids_array);
		}
	}

	return SUCCESS;
}
