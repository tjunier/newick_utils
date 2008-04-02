/* node_pos.h: structures and functions for computing node's positions. Used
 * for displaying and computing node distances. */

struct rooted_tree;

/* Used for storing the node's vertical and horizontal position - independent
 * of final medium. "Horizontal" and "vertical" are understood as if the tree
 * were displayed vertically: Vertical position is measured in leaf order or
 * fractions thereof (for inner nodes), horizontal position ("depth") is
 * measured in edge length. */

struct node_pos {
	double top;
	double bottom;
	double depth;
};

/* used for returning max depth and max label length */

struct h_data {
	int l_max; 	/* lenth of longest label */
	double d_max; 	/* depth of deepest leaf */
};

/* Allocates a node_pos structure for each node. MUST be called prior to
 * calling set_node_depth() or set_node_vpos(). */

void alloc_node_pos(struct rooted_tree *);

/* Sets the edge length information - useful when displaying, and when
 * computing length from root or from another node, etc. IMPORTANT:
 * alloc_node_pos() MUST have been called on the tree before calling this
 * function. Returns a h_data structure, which can be useful for displaying. */

struct h_data set_node_depth(struct rooted_tree *);

/* Sets the node "height" information (only useful when displaying). IMPORTANT:
 * alloc_node_pos() MUST have been called on the tree before calling this
 * function. Returns the number of  leaf nodes, which is the maximal vertical
 * position. */

int set_node_vpos(struct rooted_tree *) ;
