struct h_data;
struct rooted_tree;

/* Displays a radial tree in an SVG <g> element */

void display_svg_tree_radial(
		struct rooted_tree *tree,
		struct h_data hd,
		int align_leaves
		);
