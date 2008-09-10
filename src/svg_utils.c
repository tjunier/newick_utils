/* SVG utilities */

/* The values returned by these functions have been expermimentally determined
 * by creating SVG graphics consisting of just text, and measuring them. Of
 * course this will be OS dependent */

#include <string.h>
#include <stdlib.h>

#include "svg_utils.h"

/* Converts a font's size as a string to its size as a number. Names ("small",
 * etc) are honored. */

static int to_num_size(const char *css_font_size)
{
	int num_size = strtod(css_font_size);

	if (0 != num_size) {
		return num_size;
	} else {
		if (0 == strcmp(css_font_size, "small"))
			return 5;
		else if (0 == strcmp(css_font_size, "medium"))
			return 7;
		else if (0 == strcmp(css_font_size, "large"))
			return 10;
		else
			return -1;
	}
}

/* Returns the average width (in pixels) of a character in the given font size
 * */

double avg_char_length(const char *css_font_size)
{
	int num_font_size = to_num_size(css_font_size);

	switch(num_font_size) {
	case 5:
		return 10;
		break;	
	case 6:
		return 11;
		break;	
	default:
		return -1;
	}
}

double svg_string_length(const char *string, const char *css_font_size)
{
	int len = strlen(string);

	return len * avg_char_length(css_font_size);
}

double svg_string_height(const char *string, const char *css_font_size)
{
	int len = strlen(string);

	return len * avg_char_height(css_font_size);
}
