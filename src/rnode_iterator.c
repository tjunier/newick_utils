#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "rnode_iterator.h"
#include "hash.h"
#include "list.h"
#include "rnode.h"
#include "redge.h"

#define _GNU_SOURCE	/* needed for asprintf() */

#define SEEN "SEEN"

static const int INIT_HASH_SIZE = 1000;

struct rnode_iterator
{
	struct rnode *root;	/* starting point */
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

	iter->root = iter->current = root;
	iter->seen = create_hash(INIT_HASH_SIZE);

	return iter;
}

void destroy_rnode_iterator (struct rnode_iterator *it)
{
	destroy_hash(it->seen);
	free(it);
}

/* Returns the current node's next un-visited child, or NULL if there is none.
 * Will return NULL if node has no children. */

/* TODO: currently, this function "remembers" visited nodes (in the iter->seen
 * hash). At every call, it traverses the current node's children list until if
 * finds at child it has not seen. This takes O(n) time, for n children. Since
 * this function will be called once per child, the result is O(n²). This could
 * be improved by "remembering" the last list_elem visited in the children
 * list. Since this has a pointer to the next element, getting the next
 * unvisited child takes constant time. There just needs to be a stack of
 * "remembered" list_elem structures in struct rnode_iterator (instead of the
 * hash, which has the added benefit of using up less space). When going down
 * to a child, the current list_elem is pushed on the stack, and it is popped
 * out of it when going back up. */

/* Note however that this will only make a difference for trees that are
 * polytomous. For dichotomous trees, the present approach is probably the
 * best. */

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
		free(current_node_hash_key);
		return iter->current;
	} else {
		struct rnode *next_child = get_next_unvisited_child(iter);
		if (NULL != next_child) {
			/* proceed to next child */
			iter->current = next_child;
			free(current_node_hash_key);
			return iter->current;
		} else {
			hash_set(iter->seen, current_node_hash_key, SEEN);
			free(current_node_hash_key);
			if (iter->current == iter->root) {
				return NULL;
			} else {
				iter->current=iter->current->parent_edge->parent_node;
				return iter->current;
			}
		}
	}
}

// TODO: fix this, it doesn't work. We probably need to implement a function
// that peeks the next unvisited child, i.e.  without free()ing it from the
// 'seen' hash.

struct llist *get_nodes_in_order(struct rnode *root)
{
	struct rnode_iterator *it = create_rnode_iterator(root);
	struct hash *seen = create_hash(INIT_HASH_SIZE);
	struct rnode *current;
	struct llist *nodes_in_order = create_llist();
	char *node_hash_key;

       	node_hash_key = make_hash_key(root);
	hash_set(seen, node_hash_key, SEEN);
	prepend_element(nodes_in_order, root);
	while ((current = rnode_iterator_next(it)) != NULL) {
		char *node_hash_key = make_hash_key(current);
		printf ("current node: %s\n", current->label);
		if (NULL == hash_get(seen, node_hash_key)) {
			printf ("appending %s\n", current->label);
			prepend_element(nodes_in_order, current);
			hash_set(seen, node_hash_key, SEEN);
		}
	}
	destroy_rnode_iterator(it);

	return nodes_in_order;
}

struct hash *get_leaf_label_map(struct rnode *root)
{
	struct rnode_iterator *it = create_rnode_iterator(root);
	struct rnode *current;
	struct hash *result = create_hash(INIT_HASH_SIZE);

	while ((current = rnode_iterator_next(it)) != NULL) {
		if (is_leaf(current)) {
			if (strcmp("", current->label) != 0) {
				hash_set(result, current->label, current);
			}
		}
	}

	destroy_rnode_iterator(it);

	return result;
}
