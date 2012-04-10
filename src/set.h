/* Hash-based sets. Keys are char*, as for hashes, and values have no meaning */

#include "hash.h"

static int MEMBER = 1;

typedef struct hash set_t;

/* Creates a set object, initially empty; or NULL on failure. */

set_t* create_set();

/* Adds an element to a set; returns number of objects in set. */

int set_add(set_t *s, const char *key);

/* Returns the cardinal of a set - i.e., the number of elements */

int set_cardinal(set_t *s);

/* Returns true IFF set has the given key */

bool set_has_element(set_t *s, const char *key);
