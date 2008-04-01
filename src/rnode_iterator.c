#include <stdlib.h>
#include <stdio.h>

#include "rnode_iterator.h"
#include "hash.h"
#include "list.h"
#include "rnode.h"
#include "redge.h"

#define SEEN "SEEN"

static const int INIT_HASH_SIZE = 1000;

struct rnode_iterator
{
	struct rnode *current;
	struct hash *seen;
};

struct rnode_iterator *create_rnode_iterator(struct rnode *root)
{
	struct rnode_iterator *iter;
	iter = (struct rnode_iterator *) malloc(sizeof(struct rnode_iterator));
	if (NULL == iter) {
		perror(NULL);
		exit(EXIT_FAILURE);
	}

	iter->current = root;
	iter->seen = create_hash(INIT_HASH_SIZE);

	return iter;
}

/* Returns a unique string for the node, suitable for a hash key. */

static char * make_hash_key(struct rnode *node)
{
	char *key;
	if (-1 == asprintf(&key, "%p", node)) {
		perror(NULL);
		exit(EXIT_FAILURE);
	}
	return key;
}

/* Returns the current node's next un-visited child, or NULL if there is none.
 * Will return NULL if node has no children. */

static struct rnode * get_next_unvisited_child(struct rnode_iterator *iter)
{
	struct list_elem *elem;

	for (elem = iter->current->children->head; NULL != elem;
			elem = elem->next) {
		struct redge *child_edge = (struct redge *) elem->data;
		struct rnode *child = child_edge->child_node;
		char *node_hash_key = make_hash_key(child);
		if (NULL == hash_get(iter->seen, node_hash_key)) {
			free(node_hash_key);
			return child;
		}
		free(node_hash_key);
	}
	return NULL;	/* no unvisited child left */
}

struct rnode *rnode_iterator_next(struct rnode_iterator *iter)
{
	char *current_node_hash_key = make_hash_key(iter->current);

	if (is_leaf(iter->current)) {
		hash_set(iter->seen, current_node_hash_key, SEEN);
		iter->current = iter->current->parent_edge->parent_node;
		return iter->current;
	} else {
		struct rnode *next_child = get_next_unvisited_child(iter);
		if (NULL != next_child) {
			/* proceed to next child */
			iter->current = next_child;
			return iter->current;
		} else {
			hash_set(iter->seen, current_node_hash_key, SEEN);
			iter->current=iter->current->parent_edge->parent_node;
			return iter->current;
		}
	}
}

