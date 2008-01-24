struct rnode;
struct llist;

struct rooted_tree {
	struct rnode *root;
	struct llist *nodes_in_order;
};

/* Reroots the tree in such a way that 'outgroup' and descendants are one of
 * the root's children, and the rest of the tree is the other child. The old
 * root node gets spliced out if it has only one child. */

void reroot_tree(struct rooted_tree *tree, struct rnode *outgroup);

/* Collapses pure clades (= clades in which all leaves are of the same label)
 * into a single leaf, also of the same label */

void collapse_pure_clades(struct rooted_tree *tree);

/* Destroys a tree, releasing memory. */

void destroy_tree(struct rooted_tree *);

/* Returns the number of leaves of this tree */

int leaf_count(struct rooted_tree *);
