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

/* The following functions set the value of the node's positions. This is done
 * using callback functions. The advantage of this is that the functions below
 * do not need to know how this data is stored (it may be stored in a struct
 * attributed to rnode->data (and it can be a different structure for different
 * applications); it might be a hash, or whatever). As a consequence, these
 * functions can be used by different applications, which avoids code
 * duplication. The downside, of course, is that accessors must be supplied,
 * and a few more function calls have to be executed. */

/* Sets the nodes' vertical position, which is a dimensionless number based on
 * the ordinal number of leaves (i.e. first leaf is 1, second is 2, their
 * ancestor is 1.5 (unless it has other children), etc). Last four arguments
 * are setters and getters for an rnode's top and bottom. Returns the number of
 * leaves in the tree (useful for computing scale). */ 

int set_node_vpos_cb(struct rooted_tree *t,
		void (*set_node_top)(struct rnode *, double),
		void (*set_node_bottom)(struct rnode *, double),
		double (*get_node_top)(struct rnode *),
		double (*get_node_bottom)(struct rnode *));

/* Sets the nodes' horizontal position (depth). This is determined by parent
 * edges's lengths. Edges with empty length (NOT zero length!) are arbitrarily
 * attributed a length of 1. Returns a structure containing the tree's depth
 * (depth of the deepest leaf) and the length (in characters) of the longest
 * label. These are useful for determining horizontal scale. */

struct h_data set_node_depth_cb(struct rooted_tree *,
		void (*set_node_depth)(struct rnode *, double),
		double (*get_node_depth)(struct rnode *));
