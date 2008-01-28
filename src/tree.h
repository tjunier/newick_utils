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

/* If the tree's node have data that cannot just be free()d (i.e., they have
 * pointers to allocated memory), first free that data by iterating on the
 * nodes (tree->nodes_in_order, etc), then free the tree using this function. */

void destroy_tree_except_data(struct rooted_tree *tree);

/* Returns the number of leaves of this tree */

int leaf_count(struct rooted_tree *);
