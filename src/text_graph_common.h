
/* VT100 and UTF_8 allow the use of box-drawing characters */

enum text_graph_style {
	TEXT_STYLE_RAW,    /* no translation */
	TEXT_STYLE_COMMAS, /* '+' in angles replaced by ',' or '\'' */
	TEXT_STYLE_SLASHES, /* '+' in angles replaced by '/' or '\' */
	TEXT_STYLE_VT100,  /* VT-100 box-drawing characters */
	TEXT_STYLE_UTF_8,  /* Unicode box-drawing characters */
	TEXT_STYLE_UNKNOWN,
};
