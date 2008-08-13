/* order_tree.c - do not confuse with order.c, which is the ordering program.
 * This is just the implementation of functions used by it (and others, which
 * is why they are now separated) */

#include <string.h>
#include <stdlib.h>

#include "redge.h"
#include "rnode.h"
#include "tree.h"
#include "list.h"

int lbl_comparator(const void *a, const void *b)
{
	/* I really have trouble understanding how qsort() passes the
	 * comparands to the comparator... but thanks to GDB I figured out this
	 * one. */	
	char *a_lbl = (*(struct redge **)a)->child_node->data;
	char *b_lbl = (*(struct redge **)b)->child_node->data;

	int cmp = strcmp(a_lbl, b_lbl);
	// printf ("%s <=> %s: %d\n", a_lbl, b_lbl, cmp);

	return cmp;
}

void order_tree(struct rooted_tree *tree)
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

			struct redge ** kids_array;
			int count = current->children->count;
			kids_array = (struct redge **)
				llist_to_array(current->children);
			destroy_llist(current->children);
			qsort(kids_array, count, sizeof(struct redge *),
					lbl_comparator);
			struct llist *ordered_kids_list;
			ordered_kids_list = array_to_llist(
				(void **) kids_array, count);
			current->children = ordered_kids_list;

			current->data = kids_array[0]->child_node->data;
		}
	}
}
