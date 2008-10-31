
struct rnode;

struct redge {
	struct rnode * parent_node;
	struct rnode * child_node;
	char *length_as_string;	/* Most of the time this is enough, but... */
	double length; 		/* ...use this when numerical value is needed */
};

/* Creates a redge and returns a pointer to it (checks malloc()) or dies.
 * Argument is the edge's length, which is stored as a string but should be
 * numeric or empty (NULL will be replaced by ""). This is because we need a
 * way to store unspecified length, and cannot use numbers for this (zero is a
 * possible numeric length, and even negative numbers are sometimes used (e.g.
 * by Neighbor-Joining))
 * The argument string is copied to a local, dynamically allocated version.
 * There is no destructor function (yet?) so the copy must be free()d (and the
 * original too IFF it was dynamically allocated, of course) */

struct redge *create_redge(char *length);

/* Frees the memory used by an edge */

void destroy_redge(struct redge *);

/* A dump function, for use with dump_llist() */

void dump_redge(void *);
