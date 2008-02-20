/* Functions for creating and manipulating trees at the node level. */

/* NOTE: Tree properties such as 'nodes_in_order' will generally be invalidated
 * by these methods. You will need either to recompute them, or to avoid
 * relying on them after the call. For example, it is safe to splice out a node
 * and then call to_newick() on the tree's root. On the other hand, the list of
 * nodes in the tree will still contain the spliced-out node (which is good
 * anyway since you need it to free the spliced-out node).*/

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

/* Splices out node 'node' (which must be inner). Children are linked directly
 * to 'node''s parent. New edges have edges such that the children nodes'
 * depth is unchanged. */

/* Should NOT be used for deleting ("pruning") nodes! (will segfault) */

void splice_out_rnode(struct rnode *node);

/* Reverses an edge: parent becomes child, and vice versa. To ensure that the
 * tree still has a single meaningful root, the edge's parent node (before the
 * call) should be the tree's root (and the edge's child node will be the new
 * root). It is the caller's responsibility to ensure this. */

void reverse_redge(struct redge *edge);

/* Unlinks a node from the tree. This will only affect. Does NOT free any memory, apart from
 * elements in the children list. In particular, nodes are still available in
 * the original tree's nodes_in_order list. */ 

void unlink_node(struct rnode *);
