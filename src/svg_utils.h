/* SVG utilities */

/* It is impossible to compute the length of a string within SVG, so I provide
 * a few functions foro doing this from C programs, using quick-and-dirty
 * heuristics */

/* Returns the length of a string (in pixels) using the specified font size */

double svg_string_length(const char *string,
		const char *css_font_size);

/* Returns the height of a string (in pixels) using the specified font size */

double svg_string_height(const char *string,
		const char *css_font_size);
