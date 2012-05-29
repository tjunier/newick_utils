/* 

Copyright (c) 2009 Thomas Junier and Evgeny Zdobnov, University of Geneva
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice,
    this list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright notice,
    this list of conditions and the following disclaimer in the documentation
    and/or other materials provided with the distribution.
* Neither the name of the University of Geneva nor the names of its
    contributors may be used to endorse or promote products derived from this
    software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/
/* A simple hash table implementation. */

/* Values are arbitrary objects (void *), keys are char*. Hash size is fixed at creation. */

#define _GNU_SOURCE

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "hash.h"
#include "list.h"
#include "masprintf.h"
#include "common.h"

struct key_val_pair {
	char *key;
	void *value;
};

struct hash *create_hash(int n)
{
	struct hash *h;
	int i;

	/* allocate storage for struct hash */
	h = (struct hash *) malloc (sizeof(struct hash));
	if (NULL == h) return NULL;

	h->size = n;
	/* allocate storage for n pointers to llist */
	h->bins = (struct llist **) malloc (n * sizeof(struct llist *));
	if (NULL == h->bins) return NULL;
	/* create a llist at each position */
	for (i = 0; i < n; i++) {
		(h->bins)[i] = create_llist();
		if (NULL == (h->bins)[i]) return NULL;
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

int hash_set(struct hash *h, const char *key, void *value)
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
			return SUCCESS;
		}
	}
	/* Key not found - create new key_val_pair, fill it with key and val,
	 * and append to bin. */
	kvp = (struct key_val_pair *) malloc (sizeof (struct key_val_pair));
	if (NULL == kvp) return FAILURE;
	kvp->key = strdup(key);
	kvp->value = value;

	if (! append_element(bin, kvp)) return FAILURE;
	h->count++;

	return SUCCESS;
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
	if (NULL == list) return NULL;
	int i;

	for (i = 0; i < h->size; i++) {
	       struct llist *bin = h->bins[i];	
	       struct list_elem *el;
	       for (el = bin->head; NULL != el; el = el->next) {
		       struct key_val_pair *kvp = el->data;
		       if (! append_element(list, kvp->key))
			       return NULL;
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
	return masprintf("%p", addr);
}

