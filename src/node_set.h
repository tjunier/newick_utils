/* Functions for node sets, and related ancillary tasks. */

struct llist;
struct hash;
struct rnode;
struct rooted_tree;

/* Error codes */

enum ns_return {NS_OK, NS_DUP_LABEL, NS_EMPTY_LABEL};

typedef char* node_set;

/* Creates a node_set for 'node_count' nodes. */

node_set create_node_set(int node_count);

/* Adds node 'node_number' to set */

void node_set_add(node_set set, int node_number, int node_count);

/* returns true iff 'set' contains node number 'node_number' */

int node_set_contains(node_set set, int node_number, int node_count);

/* returns the union of two sets */

node_set node_set_union(node_set set1, node_set set2, int node_count);

/* adds set2 to set1 (which is modified) */

void node_set_add_set(node_set set1, node_set set2, int node_count);

/* Creates a label -> ordinal number map.  Returns 0 if there was a problem
 * (such as a leaf without a label, or a non-unique label; returns 1 otherwise
 * */

int build_name2num(struct rooted_tree *tree, struct hash **name2num_ptr);

/* returns a string representation of the node set (cf PHYLIP) */

char * node_set_to_s(node_set set, int node_count);
