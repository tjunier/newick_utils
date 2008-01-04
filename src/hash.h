/* Simple hash table. Arbitrary data, keyed by strings. Fixed number of bins, does not grow. */

struct llist;

struct hash {
	struct llist **bins;
	int size;
};

/* Creates a hash with n bins. If memory allocation fails, exits program. */

struct hash * create_hash(int n);

/* Inserts a (key, value) pair into a hash */

void hash_set(struct hash *, char *key, void *value);

/* Retrieves a value from a hash */

void *hash_get(struct hash *, char *key);

/* Dumps a hash on stdout. Hash bins will be dumped via dump_llist(), which
 * will be passed dump_func to dump the list contents. */

void dump_hash(struct hash *, void (*)());

/* Returns a struct llist* with all hash keys. Order is not specified. */

struct llist *hash_keys(struct hash *);
