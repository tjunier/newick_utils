struct h_data;
struct rooted_tree;

/* Sets the length of the tree's root (in pixels). This can be useful if the
 * tree is very star-like. */

void set_svg_root_length(int *);

/* Displays a radial tree in an SVG <g> element */

void display_svg_tree_radial(
		struct rooted_tree *tree,
		struct h_data hd,
		int align_leaves,
		int with_scale_bar,
		char *branch_length_unit
		);
