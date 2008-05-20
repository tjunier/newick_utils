struct rnode;
struct llist;

extern const int FREE_NODE_DATA;
extern const int DONT_FREE_NODE_DATA;

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

/* Destroys a tree, releasing memory. Node data is freed if 'free_node_data' is
 * tre (but any dynamically allocated memory pointed to by node data is NOT
 * freed), so 'free_node_data' should be set to true IFF the data can be
 * free()d directly. Otherwise, it will need to be free()d manually. */

void destroy_tree(struct rooted_tree *, int free_node_data);

/* If the tree's node have data that cannot just be free()d (i.e., they have
 * pointers to allocated memory), first free that data by iterating on the
 * nodes (tree->nodes_in_order, etc), then free the tree using this function. */

// void destroy_tree_except_data(struct rooted_tree *tree);

/* Returns the number of leaves of this tree */

int leaf_count(struct rooted_tree *);

/* Returns a list of nonempty leaf labels, in parse order */

struct llist *get_leaf_labels(struct rooted_tree *);

/* Returns a list of all nonempty labels, in parse order */

struct llist *get_labels(struct rooted_tree *);

/* Returns true iff tree is a cladogram, i.e. doesn't contain any branch length
 * information. A single nonempty branch length means that the tree is NOT a
 * cladogram (although I don't think that such a tree is very elegant).
 * Likewise, a tree with all branch lengths set to zero isn't a cladogram
 * either (exactly what it is, I don't know :-) ) */

int is_cladogram(struct rooted_tree *tree);

/* Takes a list of labels and returns the corresponding nodes, in the same
 * order */

struct llist *nodes_from_labels(struct rooted_tree *tree,
		struct llist *labels);
