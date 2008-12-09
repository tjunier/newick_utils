struct rooted_tree;
struct h_data;

extern double leaf_vskip;

/* Functions for drawing orthogonal trees */

void display_svg_tree_orthogonal (
		struct rooted_tree *tree,
		struct h_data hd,
		int align_leaves,
		int with_scale_bar,
		char *branch_length_unit
		);
