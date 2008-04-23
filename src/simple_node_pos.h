/* simple_node_pos.h: structures and functions for storing an rnode's position.
 * Used for displaying the tree as a graph, and computing node distances. */

struct rnode;
struct rooted_tree;

/* Used for storing the node's vertical and horizontal position - independent
 * of final use (dispaly, distance computations, etc). "top" and "bottom" are
 * understood as if the tree were displayed vertically: Vertical position is
 * measured in leaf order or fractions thereof (for inner nodes), "depth" is
 * measured in edge length. 
 * Function alloc_node_pos() should be used to allocate a struct node_pos for
 * each node of a tree, then functions set_node_depth_cb() and
 * set_node_vpos_cb() (see node_pos_alloc.h) should be used to compute and fill
 * in the structures' data.
 * Applications that only need to store nodes' positions should use this
 * structure. Other apps (like SVG display) who need to store other per-node
 * data should use another struct. */

struct simple_node_pos {
	double top;
	double bottom;
	double depth;
};

/* Allocates a simple_node_pos structure for each node. */

void alloc_simple_node_pos(struct rooted_tree *);

/* Setters and getters. These should be passed as arguments to
 * set_node_vpos_cb() and set_node_depth_cb() (see node_pos_alloc.h). */

void set_simple_node_pos_top (struct rnode *, double);
void set_simple_node_pos_bottom (struct rnode *, double);
void set_simple_node_pos_depth (struct rnode *, double);

double get_simple_node_pos_top (struct rnode *);
double get_simple_node_pos_bottom (struct rnode *);
double get_simple_node_pos_depth (struct rnode *);
