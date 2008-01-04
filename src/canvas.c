#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "canvas.h"

struct canvas *create_canvas(int width, int height)
{
	struct canvas *cp;
	int line_no;

	cp = malloc(sizeof(struct canvas));
	if (NULL == cp) {
		perror(NULL);
		exit(EXIT_FAILURE);
	}

	cp->width = width;
	cp->height = height;
	/* allocate space for pointers to lines */
	cp->lines = malloc(height * sizeof(char *));
	if (NULL == cp->lines) {
		perror(NULL);
		exit(EXIT_FAILURE);
	}
	/* now allocate lines */
	for (line_no = 0; line_no < height; line_no++) {
		int c;
		cp->lines[line_no] = malloc((width+1) * sizeof(char));
		if (NULL == cp->lines[line_no]) {
			perror(NULL);
			exit(EXIT_FAILURE);
		}
		for (c = 0; c < width; c++)
			cp->lines[line_no][c] = ' ';
		cp->lines[line_no][c] = '\0';
	}

	return cp;
}

void canvas_draw_hline(struct canvas *canvasp, int line, int start_col, int stop_col)
{
	int col;

	/* <= is intentional (stop position is included in line) */
	for (col = start_col; col <= stop_col; col++) 
		if ('|' == canvasp->lines[line][col])
			canvasp->lines[line][col] = '+';
		else
			canvasp->lines[line][col] = '-';
}

void canvas_draw_vline(struct canvas *canvasp, int col, int start_line, int stop_line)
{
	int line;

	assert(col >= 0);
	assert(start_line >= 0);
	assert(stop_line >= 0);

	/* <= is intentional (stop position is included in line) */
	for (line = start_line; line <= stop_line; line++)
		if ('-' == canvasp->lines[line][col])
			canvasp->lines[line][col] = '+';
		else
			canvasp->lines[line][col] = '|';
}

void canvas_write(struct canvas *canvasp, int col, int line, char *text)
{
	int text_length = strlen(text);
	int space_left = canvasp->width - col;
	int min = text_length < space_left ? text_length : space_left;
	char *dest = canvasp->lines[line]+col;

	strncpy(dest, text, min);
}

void canvas_dump(struct canvas* canvasp)
{
	int line;

	for (line = 0; line < canvasp->height; line++)
		printf("%s\n", canvasp->lines[line]);
}

void canvas_inspect(struct canvas* canvasp)
{
	int line;

	for (line = 0; line < canvasp->height; line++)
		printf("L%i: >%s<\n", line, canvasp->lines[line]);
}

void destroy_canvas(struct canvas *canvasp)
{
	int i;

	for (i = 0; i < canvasp->height; i++) {
		free(canvasp->lines[i])	;
	}

	free(canvasp->lines);
	free(canvasp);
}
