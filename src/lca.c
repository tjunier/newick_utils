#include <stdlib.h>
#include <stdio.h>

#include "tree.h"
#include "rnode.h"
#include "list.h"
#include "redge.h"

struct rooted_tree *lca2w_tree;

/* marks all nodes of the tree unseen */

void mark_unseen(struct rooted_tree *tree)
{
	struct list_elem *el;

	for (el=tree->nodes_in_order->head; NULL!=el; el=el->next) {
		struct rnode *current = (struct rnode *) el->data;
		current->data = NULL;
	}
}
/* Start from descendant A and climbs to the root, marking each node as 'seen'
 * along the way. Then starts again from descendant B, returning the first
 * 'seen' node. */

struct rnode *lca2(struct rooted_tree *tree, struct rnode *desc_A,
		struct rnode *desc_B)
{
	char *SEEN = "seen";
	
	/* Makes sure no node accidentally retains 'seen' status. */
	mark_unseen(tree);

	/* Climb to root, marking nodes as 'seen' */
	while (! is_root(desc_A)) {
		desc_A->data = SEEN;
		desc_A = desc_A->parent_edge->parent_node;
	}
	desc_A->data = SEEN;

	while (SEEN != desc_B->data)
		desc_B = desc_B->parent_edge->parent_node;

	return desc_B;
}

/* A wrapper around lca2() so it can be passed to reduce() */

void * lca2w(void *a, void *b)
{
	return lca2(lca2w_tree, (struct rnode*) a,
			(struct rnode *) b);
}

/* Returns the LCA of any number of nodes, by applying lca2 to a
 * list of descendants. */

struct rnode *lca (struct rooted_tree *tree,
		struct llist *descendants)
{
	void * result;

	/* lca2w() has only 2 args (by definition of reduce() in
	 * list.h), but lca() needs *tree */
	lca2w_tree = tree; 
	result = reduce(descendants, lca2w);

	return (struct rnode *) result;
}
