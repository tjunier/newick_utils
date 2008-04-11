/* Functions for displaying a tree as SVG. User should call svg_header(), then
 * display_svg_tree(), then svg_footer(). */

struct rooted_tree;

/* The following are for setting parameters for the SVG job. */

void set_svg_width(int);
void set_svg_inner_label_font_size(char *);
void set_svg_leaf_label_font_size(char *);
void set_svg_colormap_file(char *);

/* Writes an SVG header */

void svg_header();

/* Writes a tree into a <g> object */

void display_svg_tree(struct rooted_tree *);

/* Writes an SVG footer */

void svg_footer();
