/* functions for iterating over a rnode hierarchy given its root, and utility
 * functions that depend on those */

struct rnode;
struct rnode_iterator;
struct llist;

/* Creates an iterator. You can then pass it to the functions below. */

struct rnode_iterator *create_rnode_iterator(struct rnode *root);

/* Destroys the iterator, freeing allocated memory */

void destroy_rnode_iterator(struct rnode_iterator *);

/* Gets the next node information */

struct rnode *rnode_iterator_next(struct rnode_iterator *);

/* Returns the list of nodes that descend from the argument node, in parse
 * order. Together wit the argument node, this can be used to create a struct
 * rooted_tree. */

struct llist *get_nodes_in_order(struct rnode *);

/* Returns a label->rnode map of all leaves that descend from 'root' */

struct hash *get_leaf_label_map(struct rnode *root);
