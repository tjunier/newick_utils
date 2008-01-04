#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "list.h"
#include "rnode.h"

struct node_map {
	struct rnode **rnode_ptr_array;
	int key_count;
};

/* Comparison function, used for sorting the array of struct rnode *. */

int rnode_cmp (const void *a, const void *b)
{
	/* Note that what we are passed are struct node **: rnode_ptr_array,
	 * rnode_ptr_array + 1, etc. The values of these array elements are the
	 * pointers to the rnodes themselves, hence the double indirection. */

	/* Debug note: At first I had mistakenly declared 'struct rnode
	 * *node_a', etc (pointer to rnode instead of pointer to pointer to
	 * rnode). Of course, it segfaulted as soon as I tried to access the
	 * rnode's label. I finally  got it by looking at the addresses of the
	 * arguments, which were of course NOT addresses of struct rnodes but of
	 * elements in the array of rnode  pointers */

	struct rnode **node_a = (struct rnode **) a;
	struct rnode **node_b = (struct rnode **) b;

	return strcmp((*node_a)->label, (*node_b)->label);
}

/* Comparison function, used for retrieving a struct rnode * from the array.
 * The first element is a string which will be compared to the node's labels.
 * See bsearch(3). */

int key2rnode_cmp (const void *k, const void *a)
{
	struct rnode **node_a = (struct rnode **) a;
	char * key = (char *) k;

	return strcmp(key, (*node_a)->label);
}

struct node_map * create_node_map(struct llist *node_list)
{
	struct node_map *map;
	struct list_elem *elem;
	int i;
	
	map = malloc(sizeof(struct node_map));
	if (NULL == map) {
		perror(NULL);
		exit(EXIT_FAILURE);
	}
	map->key_count = node_list->count;
	map->rnode_ptr_array = malloc(map->key_count * sizeof(struct rnode *));
	if (NULL == map->rnode_ptr_array) {
		perror(NULL);
		exit(EXIT_FAILURE);
	}

	
	for (i=0, elem=node_list->head; NULL!=elem; elem=elem->next, i++) {
		map->rnode_ptr_array[i] = (struct rnode *) elem->data;
	}

	qsort(map->rnode_ptr_array, map->key_count, sizeof(struct rnode*),
		rnode_cmp);

	return map;
}

struct rnode *get_node_with_label(const struct node_map *map,
		const char *label)
{
	struct rnode **result;

	result = bsearch((void *) label,  map->rnode_ptr_array,
			(size_t) map->key_count,
			(size_t) sizeof(struct rnode *), key2rnode_cmp);
	if  (NULL == result)
		return NULL;	/* not found */
	else 
		return *result;
}

void dump_map(const struct node_map *map)
{
	int i;

	for (i = 0; i < map->key_count; i++) {
		struct rnode *node;
		node = map->rnode_ptr_array[i];
		printf ("%p\t%s\n", node, node->label);
	}
}
