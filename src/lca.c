#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "tree.h"
#include "rnode.h"
#include "list.h"
#include "redge.h"
#include "hash.h"
#include "nodemap.h"

struct rooted_tree *lca2w_tree;

/* NOTE: these two functions are obsolete, but I keep them in case the new
 * implementation turns out to be faulty. */

/* marks all nodes of the tree unseen */

/*
void mark_unseen(struct rooted_tree *tree)
{
	struct list_elem *el;

	for (el=tree->nodes_in_order->head; NULL!=el; el=el->next) {
		struct rnode *current = (struct rnode *) el->data;
		current->data = NULL;
	}
}
*/

/* Old version of lca2(). Used rnode->data to mark nodes, the new version uses
 * a hash (and thus avoids modifying its arguments, but may be somewhat slower) */

/*
struct rnode *old_lca2(struct rooted_tree *tree, struct rnode *desc_A,
		struct rnode *desc_B)
{
	char *SEEN = "seen";
	
	// Makes sure no node accidentally retains 'seen' status. 
	// mark_unseen(tree);

	// Climb to root, marking nodes as 'seen' 
	while (! is_root(desc_A)) {
		desc_A->data = SEEN;
		desc_A = desc_A->parent_edge->parent_node;
	}
	desc_A->data = SEEN;

	while (SEEN != desc_B->data)
		desc_B = desc_B->parent_edge->parent_node;

	return desc_B;
}
*/


/* Start from descendant A and climbs to the root, marking each node as 'seen'
 * along the way. Then starts again from descendant B, returning the first
 * 'seen' node. */

struct rnode *lca2(struct rooted_tree *tree, struct rnode *desc_A,
		struct rnode *desc_B)
{
	char *SEEN = "seen";
	char *key;

	/* This hash will remember which nodes have been visited. It need be at
	 * most as large as the number of nodes in the tree. */
	struct hash *seen_nodes = create_hash(tree->nodes_in_order->count);

	/* Climb to root, marking nodes as 'seen' */
	while (! is_root(desc_A)) {
		key = make_hash_key(desc_A);
		hash_set(seen_nodes, key, SEEN);
		free(key);
		desc_A = desc_A->parent_edge->parent_node;
	}
	key = make_hash_key(desc_A);
	hash_set(seen_nodes, key, SEEN);
	free(key);

	while (1) {
		key = make_hash_key(desc_B);
		if (NULL != hash_get(seen_nodes, key)) { /* seen */
			free(key);
			break;
		}
		free(key);
		desc_B = desc_B->parent_edge->parent_node;
	}

	destroy_hash(seen_nodes);

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

struct rnode *lca_from_nodes (struct rooted_tree *tree,
		struct llist *descendants)
{
	struct rnode *result;
	struct llist *copy = shallow_copy(descendants);

	result = lca(tree, copy);

	destroy_llist(copy);
	return result;
}

struct rnode *lca_from_labels(struct rooted_tree *tree, struct llist *labels)
{
	struct hash *node_map = create_label2node_map(tree->nodes_in_order);
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

