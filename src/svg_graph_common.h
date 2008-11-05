/* Declarations used by all SVG modules */

#define ROOT_SPACE 10			/* pixels */
#define LBL_SPACE 10			/* pixels */
#define INNER_LBL_SPACE 4		/* pixels */
#define CHAR_WIDTH 5			/* pixels, approx. for 'medium' fonts */
#define edge_length_v_offset -4 	/* pixels */
#define URL_MAP_SIZE 100		/* bins */
#define UNSTYLED_CLADE 0

extern struct hash *url_map;
extern char *leaf_label_class;
extern char *inner_label_class;
extern int graph_width;
extern int svg_whole_v_shift; 

/* rnode data for SVG trees */

struct svg_data {
	double top;
	double bottom;
	double depth;
	int clade_nb;	/* For attributing styles */
	/* ... other node properties ... */
};

/* The following are setters, their function should be obvious */

void set_svg_CSS_map_file(FILE *);
void set_svg_leaf_label_style(char *);
void set_svg_inner_label_style(char *);
void set_svg_edge_label_style(char *);

/* Returns the largest power of ten not greater than argument */

double largest_PoT_lte(double);
