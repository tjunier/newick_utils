struct rnode;
struct redge;

/* Adds edge 'edge' to 'parent''s list of children edges */

void add_child_edge(struct rnode *parent, struct redge *edge);

/* sets 'edge' as 'child''s parent edge, and 'child' as 'edge''s child
 * node. */

void set_parent_edge(struct rnode *child, struct redge *edge);

/* Links 'parent' and 'child'. An edge of length 'length' is created. */

void link_p2c(struct rnode *parent, struct rnode *child, char *length);

/* Inserts a node withe label 'label' in this node's parent edge. The two
 * resulting edges will each have a length of half the original's, if
 * specified. */

void insert_node_above(struct rnode *node, char *label);

/* Replaces edge 'old_edge' by edge 'new_edge'. Does nothing if 'old_edge' is not found. */

void replace_child_edge(struct rnode *node, struct redge *old, struct redge *new);

/* Splices out node 'node'. Any children are linked directly to 'node''s
 * parent. New edges have edges such that the children nodes' depth is
 * unchanged. */

void splice_out_rnode(struct rnode *this);

/* Reverses an edge: parent becomes child, and vice versa. To ensure that the
 * tree still has a single meaningful root, the edge's parent node (before the
 * call) should be the tree's root (and the edge's child node will be the new
 * root). It is the caller's responsibility to ensure this. */

void reverse_redge(struct redge *edge);
