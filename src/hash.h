/* Simple hash table. Arbitrary data, keyed by strings. Fixed number of bins, does not grow. */

struct llist;

struct hash {
	struct llist **bins;
	int size;	/* the number of bins */
	int count;	/* the number of data elements - initially 0 */
};

struct key_val_pair {
	char *key;
	void *value;
};

/* Creates a hash with n bins. If memory allocation fails, exits program. */

struct hash * create_hash(int n);

/* Inserts a (key, value) pair into a hash. Increments count. */

void hash_set(struct hash *, const char *key, void *value);

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
 * key. */

char *make_hash_key(void *addr);
