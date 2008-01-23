/* Functions for node sets, and related ancillary tasks. */

struct llist;
struct hash;
struct rnode;
struct rooted_tree;

/* Error codes */

enum ns_return {NS_OK, NS_DUP_LABEL, NS_EMPTY_LABEL};

typedef char* node_set;

/* Creates a node set that contains a single node (number 'node_number') among
 * 'node_count' possible nodes. */

node_set create_node_set(int node_number, int node_count);

/* Creates the list of node sets for all of this node's children. Second arg
 * 'lbl2num' maps the node's label to a number (see build_name2num() below);
 * 'c' is the total number of nodes in the tree. Assumes this node is NOT a
 * leaf. Returns the list of node sets (NOT their union). The use of a
 * label->number mapping enables a consistent numbering over many trees (since
 * the order of leaves will generally be different from tree to tree).*/

struct llist *children_node_set_list(struct rnode* n, struct hash *lbl2num, int c);

/* Creates a label -> ordinal number map.  Returns 0 if there was a problem
 * (such as a leaf without a label, or a non-unique label; returns 1 otherwise
 * */

int build_name2num(struct rooted_tree *tree, struct hash **name2num_ptr);
