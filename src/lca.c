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
 * list of descendants. THIS USES UP THE LIST!*/

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

struct rnode *lca_from_labels(struct rooted_tree *tree, struct llist *labels)
{
	struct hash *node_map = create_node_map(tree->nodes_in_order);
	struct llist *descendant_nodes = create_llist();
	struct list_elem *el;

	for (el = labels->head; NULL != el; el = el->next) {
		char *label = (char *) el->data;
		if (0 == strcmp("", label)) {
			fprintf(stderr, "WARNING: empty label.\n");
			continue;
		}
		struct rnode *desc = hash_get(node_map, label);
		if (NULL == desc) {
			fprintf(stderr, "WARNING: no node with label '%s'.\n",
					label);
			continue;
		}
		append_element(descendant_nodes, desc);
	}

	struct rnode *result = lca(tree, descendant_nodes);

	destroy_hash(node_map);
	destroy_llist(descendant_nodes);

	return result;
}

