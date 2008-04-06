/* Functions for displaying a tree as SVG. User should call svg_header(), then display_svg_tree(), then svg_footer(). */

struct rooted_tree;

/* Writes an SVG header */

void svg_header();

/* Writes a tree into a <g> object */

void display_svg_tree(struct rooted_tree *tree, int width);

/* Writes an SVG footer */

void svg_footer();
