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
/* Simple hash table. Arbitrary data, keyed by strings. Fixed number of bins, does not grow. */

/* NOTE: All functions except the most simple ones can fail, in which case the
 * return value indicates failure or success. Functions that return pointers
 * will return NULL in case of error (most probably a malloc() problem due to
 * insufficient memory. Functions that just perform an action (i.e., who could
 * return void) return SUCCESS or FAILURE. FAILURE will often be caused by
 * insufficient memory in a called function. 
 */

struct llist;

/**  \todo this might be made private. */

/** A simple hash table. The number of bins is fixed at the time the structure
 * is created. */

struct hash {
	struct llist **bins;	/**< the bins */
	int size;	/**< the number of bins */
	int count;	/**< the number of data elements - initially 0 */
};

/* Creates a hash with n bins. If memory allocation fails, returns NULL . */

struct hash * create_hash(int n);

/* Inserts a (key, value) pair into a hash. Increments count. The 'key' will be
 * duplicated. */

int hash_set(struct hash *, const char *key, void *value);

/* Retrieves a value from a hash. Returns the value, or NULL if not present. */

void *hash_get(struct hash *, const char *key);

/* Dumps a hash on stdout. if 'dump_func' is not NULL, it will be used to
 * display the value of each key-value pair. */

void dump_hash(struct hash *, void (*dump_func)());

/* Returns a struct llist* with all hash keys. Order is not specified. */

struct llist *hash_keys(struct hash *);

/* Destroys a hash (but does NOT destroy its contents - iterate on keys for that) */

void destroy_hash(struct hash *);

/* Returns a string representation of an address, suitable for use as a hash
 * key. Allocates storage, use free() when no longer needed. */

char *make_hash_key(void *addr);
