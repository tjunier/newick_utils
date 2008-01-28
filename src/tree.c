#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "tree.h"
#include "link.h"
#include "redge.h"
#include "rnode.h"
#include "list.h"

void reroot_tree(struct rooted_tree *tree, struct rnode *outgroup)
{
	struct rnode *old_root = tree->root;

	/* Insert node (new root) above outgroup */
	insert_node_above(outgroup, "");
	struct rnode *new_root = outgroup->parent_edge->parent_node;
	
	/* Invert edges from old root to new root (i.e., the tree is always in
	 * a consistent state) */
	/* First, we go from the new root to the old, prepending edges to a list */
	struct llist *revert_list = create_llist();
	struct rnode *node;
	for (node = new_root; ! is_root(node); node = node->parent_edge->parent_node) {
		struct redge *edge = node->parent_edge;
		prepend_element(revert_list, edge);
	}
	/* Then, we reverse the edges in the list. */
	struct list_elem *elem;
	for (elem = revert_list->head; NULL != elem; elem = elem->next) {
		struct redge *edge = (struct redge*) elem->data;
		reverse_redge(edge);
	}

	splice_out_rnode(old_root);

	tree->root = new_root;
}

/* Returns true IFF all children are leaves. Assumes n is not a leaf. */

int all_children_are_leaves(struct rnode *n)
{
	struct list_elem *el;
	for (el = n->children->head; NULL != el; el = el->next) {
		struct rnode *child = ((struct redge *)el->data)->child_node;
		if (! is_leaf(child)) return 0;
	}

	return 1;
}

/* Returns true IFF all children have the same label. If true, sets 'label' to
 * the shared label. Assumes n is inner node, and all its children are leaves. */

int all_children_have_same_label(struct rnode *n, char **label)
{

	/* get first child's label */

	struct list_elem *el = n->children->head;
	struct rnode *child = ((struct redge *)el->data)->child_node;
	char *ref_label = child->label;

	/* iterate over other children, and compare their label to the first's */

	for (el = el->next; NULL != el; el = el->next) {
		child = ((struct redge *)el->data)->child_node;
		if (0 != strcmp(ref_label, child->label))
			return 0; /* found a different label */
	}

	*label = ref_label;
	return 1;
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
			/* set own label to children's label */
			current->label = label;
			/* remove children */
			/* TODO: write a function clear(struct llist*) that
			 * does this (and frees the memory, too) */
			current->children->head = NULL;
			current->children->tail = NULL;
			current->children->count = 0;
		}
	}
}

void destroy_tree(struct rooted_tree *tree)
{
	struct list_elem *e;

	/* Traversing in parse order ensures that children list's data are
	 * already empty when we destroy the list (since the lists contain
	 * children edges) */
	for (e = tree->nodes_in_order->head; NULL != e; e = e->next) {
		struct rnode *current = e->data;
		destroy_llist(current->children);
		free(current->parent_edge->length_as_string);
		free(current->parent_edge);
		free(current->label);
		free(current->data);	/* only works if data has no pointer to allocated space! */
		free(current);
	}

	destroy_llist(tree->nodes_in_order);
	free(tree);
}

/* TODO: refactor. This f() is almost a clone of the previous one. Make
 * wrappers, or something. */

void destroy_tree_except_data(struct rooted_tree *tree)
{
	struct list_elem *e;

	/* Traversing in parse order ensures that children list's data are
	 * already empty when we destroy the list (since the lists contain
	 * children edges) */
	for (e = tree->nodes_in_order->head; NULL != e; e = e->next) {
		struct rnode *current = e->data;
		destroy_llist(current->children);
		free(current->parent_edge->length_as_string);
		free(current->parent_edge);
		free(current->label);
		free(current);
	}

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
