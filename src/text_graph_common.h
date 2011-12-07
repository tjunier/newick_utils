
/* VT100 and UTF_8 allow the use of box-drawing characters */

enum graph_style {
	ASCII_RAW,	/* no translation */
	ASCII_ART_1,	/* '+' in angles replaced by ',' or '\'' */
	VT100,		/* VT-100 box-drawing characters */
	UTF_8		/* Unicode box-drawing characters */
};
