#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "list.h"
#include "rnode.h"
#include "hash.h"

struct hash * create_label2node_map(struct llist *node_list)
{
	struct hash *map;
	struct list_elem *elem;
	
	map = create_hash(node_list->count);

	for (elem = node_list->head; NULL != elem; elem = elem->next) {
		struct rnode *current = (struct rnode *) elem->data;
		if (strcmp("", current->label) == 0) { continue; }
		hash_set(map, current->label, current);
	}

	return map;
}
