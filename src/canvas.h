
struct canvas {
	int width;
	int height;
	char **lines;
	};

/* Creates a canvas of w chars by h lines */

struct canvas *create_canvas(int w, int l);

/* Prints the canvas lines */

void print(struct canvas*);

/* Draws a horizontal line on the canvas, on line 'line', from column 'start'
 * to column 'end'. The line is made of '-', except if there already is a '|',
 * in which case a '+' is written. */

void canvas_draw_hline(struct canvas*, int line, int start, int end);

/* Draws a vertical line on the canvas, on column 'col', form line 'start' to
 * line 'end'. The line is made of '|', except if there already is a '-',
 * in which case a '+' is written.  */

void canvas_draw_vline(struct canvas*, int col, int start, int end);

/* Writes the string, starting at (col, line) */

void canvas_write(struct canvas*, int col, int line, char *text);

/* Dumps the canvas onto stdout */

void canvas_dump(struct canvas*);

/* (Debugging) like canvas_dump(), but more info */

void canvas_inspect(struct canvas* canvasp);

/* Releases all memory used by the canvas. Don't use it after this! */

void destroy_canvas(struct canvas*);
