/* A simple hash table implementation. */

/* Values are arbitrary objects (void *), keys are char*. Hash size is fixed at creation. */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "hash.h"
#include "list.h"

struct hash *create_hash(int n)
{
	struct hash *h;
	int i;

	/* allocate storage for struct hash */
	h = (struct hash *) malloc (sizeof(struct hash));
	if (NULL == h) {
		perror(NULL);
		exit(EXIT_FAILURE);
	}

	h->size = n;
	/* allocate storage for n pointers to llist */
	h->bins = (struct llist **) malloc (n * sizeof(struct llist *));
	if (NULL == h->bins) {
		perror(NULL);
		exit(EXIT_FAILURE);
	}
	/* create a llist at each position */
	for (i = 0; i < n; i++) {
		(h->bins)[i] = create_llist();
	}
	h->count = 0; 	/* no key-value paits yet */
	return h;
}

/* Bernstein's hash function. See e.g.
 * http://www.eternallyconfuzzled.com/tuts/algorithms/jsw_tut_hashing.aspx */

unsigned int hash_func (const char *key)
{
	int h=0;
	while(*key) h=33*h + *key++;
	return h;
}

void hash_set(struct hash *h, const char *key, void *value)
{
	int hash_code = hash_func(key) % h->size;
	struct llist *bin;
	struct key_val_pair *kvp;

	bin = (h->bins)[hash_code];

	/* First, see if key is already in bin. If so, just replace value. */
	struct list_elem *le;
	for (le = bin->head; NULL != le; le = le->next) {
		kvp = (struct key_val_pair *) le->data;
		if (0 == strcmp(key, kvp->key)) {
			kvp->value = value;
			return;
		}
	}
	/* Key not found - create new key_val_pair, fill it with key and val,
	 * and append to bin. */
	kvp = (struct key_val_pair *) malloc (sizeof (struct key_val_pair));
	if (NULL == kvp) {
		perror(NULL);
		exit(EXIT_FAILURE);
	}
	kvp->key = strdup(key);
	kvp->value = value;

	append_element(bin, kvp);
	h->count++;
}

void *hash_get(struct hash *h, const char *key)
{
	int hash_code = hash_func(key) % h->size;
	struct llist *bin;
	struct key_val_pair *kvp;

	bin = (h->bins)[hash_code];
	struct list_elem *le;
	for (le = bin->head; NULL != le; le = le->next) {
		kvp = (struct key_val_pair *) le->data;
		if (0 == strcmp(key, kvp->key)) {
			return kvp->value;
		}
	}
	return NULL; /* not found */
}

void dump_hash(struct hash *h, void (*dump_func)())
{
	int i;

	printf ("Dump of hash at %p: (%d bins, %d pairs):\n", h, h->size,
			h->count);
	for (i = 0; i < h->size; i++) {
		struct list_elem *el = h->bins[i]->head;
		if (NULL == el) continue;
		printf ("Hash code: %d (%d pairs)\n", i, h->bins[i]->count);
		for (; NULL != el; el = el->next) {
			struct key_val_pair *kp;
			kp = (struct key_val_pair *) el->data;
			printf("key: %s\n", kp->key);
			if (NULL != dump_func) dump_func(kp->value);
		}
	}	

	printf ("Dump done.\n");
}

struct llist *hash_keys(struct hash *h)
{
	struct llist *list = create_llist();
	int i;

	for (i = 0; i < h->size; i++) {
	       struct llist *bin = h->bins[i];	
	       struct list_elem *el;
	       for (el = bin->head; NULL != el; el = el->next) {
		       struct key_val_pair *kvp = el->data;
		       append_element(list, kvp->key);
	       }
	}

	return list;
}

void destroy_hash(struct hash *h)
{
	int i;

	/* free internal structure */
	for (i = 0; i < h->size; i++) {
		/* free list, including key-val pairs */
		struct llist *list;
		struct list_elem *el;
		list = (h->bins)[i];
		for (el = list->head; NULL != el; el = el -> next) {
			struct key_val_pair *kvp = (struct key_val_pair *) el->data;
			char * key = kvp->key;
			free(key); /* key is a strdup()licate: free() it */
			free(kvp); /* we do NOT free value */
		}
		destroy_llist(list);
	}
	free(h->bins);
	/* free self */
	free(h);
}

/* Returns a unique string for the node, suitable for a hash key. Storage is
 * allocated and must be free()d by the user. */

char * make_hash_key(void *addr)
{
	char *key;
	if (-1 == asprintf(&key, "%p", addr)) {
		perror(NULL);
		exit(EXIT_FAILURE);
	}
	return key;
}

