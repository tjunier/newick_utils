
struct rnode;

struct redge {
	struct rnode * parent_node;
	struct rnode * child_node;
	double length; /* TODO: do we use this? */
	char *length_as_string;
};

/* Creates a redge and returns a pointer to it (checks malloc()) or dies.
 * Argument is the edge's length, which is stored as a string but should be
 * numeric or empty (NULL will be replaced by ""). This is because we need a
 * way to store unspecified length, and cannot use numbers for this (zero is a
 * possible numeric length, and even negative numbers are sometimes used (e.g.
 * by Neighbor-Joining)). */

struct redge *create_redge(char *length);

/* A dump function, for use with dump_llist() */

void dump_redge(void *);
