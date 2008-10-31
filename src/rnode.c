#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>

#include "rnode.h"
#include "list.h"
#include "redge.h"
#include "rnode_iterator.h"
#include "hash.h"

struct rnode *create_rnode(char *label)
{
	struct rnode *node_p;

	node_p = malloc(sizeof(struct rnode));
	if (NULL == node_p) {
		perror(NULL);
		exit(1);
	}
	if (NULL == label) {
		label = "";
	}
	node_p->label = strdup(label);
	node_p->children = create_llist();	
	node_p->parent_edge = NULL;

	node_p->data = NULL;

#ifdef SHOW_RNODE_CREATE
	fprintf(stderr, "creating rnode '%s' at %p\n", node_p->label,
			node_p);
#endif

	return node_p;
}

int children_count(struct rnode *node)
{
	return node->children->count;
}

int is_leaf(struct rnode *node)
{
	return 0 == node->children->count;
}

int is_root(struct rnode *node)
{
	if (NULL == node->parent_edge)
		return 1;
	if (NULL == node->parent_edge->parent_node)
		return 1;
	return 0;
}

int is_inner_node(struct rnode *node)
{
	return 	(!is_leaf(node) && !is_root(node));
}

void dump_rnode(void *arg)
{
	struct rnode *node = (struct rnode *) arg;

	printf ("rnode at %p: %s\n", node, node->label);
}

void free_descendants(struct rnode *node)
{
	const int HASH_SIZE = 1000; 	/* pretty arbitrary */
	struct hash *to_free = create_hash(HASH_SIZE);
	struct rnode_iterator *it = create_rnode_iterator(node);
	struct rnode *current;

	// Iterates through the tree nodes, "remembering" nodes seen for the
	// first time
	while (NULL != (current = rnode_iterator_next(it))) {
		char *node_hash_key = make_hash_key(current);
		if (NULL == hash_get(to_free, node_hash_key))
			hash_set(to_free, node_hash_key, current);
                free(node_hash_key);
	}

	// Frees all nodes "seen" above - which must be all the tree's nodes.
	struct llist *keys = hash_keys(to_free);
       	struct list_elem *el;
	for (el = keys->head; NULL != el; el = el->next) {
		char *key = el->data;
		current = hash_get(to_free, key);
		if (NULL != current->parent_edge)
			destroy_redge (current->parent_edge);
                /* these 3 should never be NULL (see create_rnode() above) */
                destroy_llist(current->children);
                // fprintf (stderr, " freeing rnode '%s' at %p\n", current->label, current);
                free(current->label);
		free(current);
	}	

        destroy_llist(keys);
        destroy_hash(to_free);
	destroy_rnode_iterator(it);
}
