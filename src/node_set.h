/* Functions for node sets. */

struct llist;

typedef int[] node_set;

/* Creates a node set that contains a single node (number 'node_number') among
 * 'node_count' possible nodes. */

int *create_node_set(int node_number, int node_count);

/* Creates the list of node sets for all of this node's children. Second arg
 * 'c' is the total number of nodes in the tree. Assumes this node is NOT a
 * leaf. Returns the list of node sets (NOT their union). */

struct llist *children_node_set(struct rnode* n, int c);
