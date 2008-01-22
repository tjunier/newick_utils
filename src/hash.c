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

	return h;
}

/* Bernstein's hash function */

unsigned int hash_func (const char *key)
{
	int h=0;
	while(*key) h=33*h + *key++;
	return h;
}

void hash_set(struct hash *h, char *key, void *value)
{
	int hash_code = hash_func(key) % h->size;
	struct llist *bin;
	struct key_val_pair *kvp;

	kvp = (struct key_val_pair *) malloc (sizeof (kvp));
	if (NULL == kvp) {
		perror(NULL);
		exit(EXIT_FAILURE);
	}
	kvp->key = key;
	kvp->value = value;

	bin = (h->bins)[hash_code];
	append_element(bin, kvp);
}

void *hash_get(struct hash *h, char *key)
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

	printf ("Dump of hash at %p: (%d bins):\n", h, h->size);
	for (i = 0; i < h->size; i++) {
		struct list_elem *el = h->bins[i]->head;
		if (NULL == el) continue;
		printf ("Hash code: %d\n", i);
		for (; NULL != el; el = el->next) {
			struct key_val_pair *kp;
			kp = (struct key_val_pair *) el->data;
			printf("key: %s\n", kp->key);
			dump_func(kp->value);
		}
	}	
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
