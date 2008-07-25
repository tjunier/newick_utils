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

/* Removes a node from its parent's children list (more precisely, remove the
 * node's parent_edge from the children list). The edge and node are not freed.
 * If the removed node had more than one sibling, the tree is in a coherent
 * state and the function stops here. If not, the tree is incoherent because
 * the parent has only one child. There are two possibilities: i) the parent is
 * the root - the function stops there and returns the root's (only) child so
 * that it may become the tree's root (we cannot do this here, since we're
 * working at node level, not tree level); ii) the parent is an inner node - it
 * gets spliced out.
 * RETURN VALUE: the root's remaining child if the removed node's parent is
 * root AND the root has only one remaining child; NULL otherwise. */

struct rnode * unlink_rnode(struct rnode *);

/* Returns the node's list of siblings. Siblings appear in the same order as in
 * the parent's children list. The list is empty for root, it may be empty for
 * leaves (it's not illegal for a node to have just one child), but this is
 * unusual. */

struct llist *siblings(struct rnode *);
