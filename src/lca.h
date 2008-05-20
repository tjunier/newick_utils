struct rooted_tree;
struct rnode;
struct llist;

/* Given a tree and two nodes, returns their last common ancestor.
 * NOTE: Both nodes are assumed to belong to the tree; if this is
 * not the case, then the function will return the tree's root or
 * may hang.  NOTE: Clobbers node->data. */

struct rnode *lca2(struct rooted_tree *, struct rnode *,
		struct rnode *);

/* Given a tree and a list of (pointers to) nodes, returns the last
 * common ancestor of the nodes. NOTE: All nodes in the list are
 * assumed to belong to the tree, otherwise behaviour is unspecified
 * and can hang. NOTE: Clobbers node->data. */

/* TODO: this function changes its argument. Make it static, and use
 * lca_from_nodes() instead. */

struct rnode *lca(struct rooted_tree *, struct llist *);

/* Given a tree and a list of nodes, returns the LCA */

struct rnode *lca_from_nodes(struct rooted_tree *tree, struct llist *labels);

/* Given a tree and a list of labels, returns the LCA */

struct rnode *lca_from_labels(struct rooted_tree *tree, struct llist *labels);
