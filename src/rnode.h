
struct redge;
struct llist;

struct rnode {
	struct redge *parent_edge;
	struct llist *children;
	char *label;
	void *data;	/* app-dependent data for this node */
};

/* allocates a rnode and returns a pointer to it, or exits. If 'label' is NULL,
 * the node will have an empty string for a label.*/

struct rnode *create_rnode(char *label);

/* returns the number of children a node has. */

int children_count(struct rnode *node);

/* returns true IFF node is a leaf */

int is_leaf(struct rnode *node);

/* returns true IFF node is root, which is defined to be one of i) node has
 * NULL parent edge, or ii) node has a parent edge, but this has a NULL parent
 * node. */

int is_root(struct rnode *node);

/* Returns true IFF node is neither a leaf nor the root */

int is_inner_node(struct rnode *node);

/* Writes the node's address and label to stdout. Argument is void* so that
 * this function can be passed to dump_llist(). */

void dump_rnode(void *arg);

/* Returns true IFF node is a "stair" node */

int is_stair_node(struct rnode *node);

