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

struct hash * create_label2node_list_map(struct llist *node_list)
{
	/* At most there will be one hash element per list element, so this
	 * will be enough. */
	struct hash *map = create_hash(node_list->count);	

	struct list_elem *elem;

	for (elem = node_list->head; NULL != elem; elem = elem->next) {
		struct rnode *current = elem->data;
		char *current_lbl = current->label;
		/* See if we have already seen this label: if so there is a
		 * list for it in 'map' */
		struct llist *lbl_node_list = hash_get(map, current_lbl);
		if (NULL == lbl_node_list) {
			/* Create the list and add * it to the hash */
		 	lbl_node_list = create_llist();
			hash_set(map, current_lbl, lbl_node_list);
		}
		/* Now add the current node to the list */
		append_element (lbl_node_list, current);
	}

	return map;
}
