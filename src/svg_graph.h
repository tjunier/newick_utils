enum { SVG_ORTHOGONAL, SVG_RADIAL };

/* Functions for displaying a tree as SVG. User should call any of the
 * set_svg_*() functions that are needed, then svg_init(), then svg_header(),
 * display_svg_tree(), and finally svg_footer(). */

struct rooted_tree;

/* The following are for setting parameters for the SVG job. */

void set_svg_width(int);
void set_svg_inner_label_font_size(char *);
void set_svg_leaf_label_font_size(char *);
void set_svg_colormap_file(char *);
void set_svg_leaf_vskip(double);
void set_svg_whole_v_shift(int);
void set_svg_branch_length_font_size(char *);
void set_svg_style(int);

/* Call this before calling svg_header(), etc, but _after_ the set_svg*()
 * functions. It will launch the initializations liek reading the color map,
 * etc. */

void svg_init();

/* Writes an SVG header */

void svg_header();

/* Writes a tree into a <g> object. if 'align_leaves' is true, the leaves will
 * be aligned (use this for cladograms). 'style' is either SVG_ORTHOGONAL or
 * SVG_RADIAL. */

void display_svg_tree(struct rooted_tree *, int align_leaves);

/* Writes an SVG footer */

void svg_footer();
