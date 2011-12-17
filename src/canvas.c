/* 

Copyright (c) 2009 Thomas Junier and Evgeny Zdobnov, University of Geneva
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice,
    this list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright notice,
    this list of conditions and the following disclaimer in the documentation
    and/or other materials provided with the distribution.
* Neither the name of the University of Geneva nor the names of its
    contributors may be used to endorse or promote products derived from this
    software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>

#include "text_graph_common.h"

// TODO: 'canvasp' is the name I used years ago when I wanted to give special
// names to pointers, something I quit doing very early anyway. Rename this to
// 'canvas' or just 'c', as it is rather obvious.

enum canvas_type { CANVAS_TYPE_RAW, CANVAS_TYPE_VT100 };
enum plus_type { UPPER_ANGLE, LOWER_ANGLE, CROSS, TEE, UNKNOWN };

static const char *VT_BEG = "\033(0\017";
static const char *VT_END = "\033(B";

struct canvas;

static void raw_canvas_draw_hline(struct canvas *canvasp, int line, int start_col, int stop_col);
static void raw_canvas_draw_vline(struct canvas *canvasp, int col, int start_line, int stop_line);
static void raw_canvas_draw_upper_corner(struct canvas *canvasp, int col, int line, char symbol);
static void raw_canvas_draw_lower_corner(struct canvas *canvasp, int col, int line, char symbol);
static void raw_canvas_write(struct canvas *canvasp, int col, int line, char *text);
static void raw_canvas_dump(struct canvas *canvasp);

static void vt100_canvas_draw_hline(struct canvas *canvasp, int line, int start_col, int stop_col);
static void vt100_canvas_draw_vline(struct canvas *canvasp, int col, int start_line, int stop_line);
static void vt100_canvas_draw_upper_corner(struct canvas *canvasp, int col, int line, char symbol);
static void vt100_canvas_draw_lower_corner(struct canvas *canvasp, int col, int line, char symbol);
static void vt100_canvas_write(struct canvas *canvasp, int col, int line, char *text);
static void vt100_canvas_dump(struct canvas *canvasp);

struct canvas {
	enum canvas_type type;
	int width;
	int height;
	char **lines;
	void (*draw_hline)(struct canvas *self, int width, int start_pos, int stop_pos);
	void (*draw_vline)(struct canvas *self, int col, int start_line, int stop_line);
	void (*write)(struct canvas *self, int col, int line, char *text); 
	void (*draw_upper_corner)(struct canvas *self, int col, int width, char symbol);
	void (*draw_lower_corner)(struct canvas *self, int col, int width, char symbol);
	void (*dump)(struct canvas *self);
};

static struct canvas *create_canvas(int width, int height, enum canvas_type type)
{
	struct canvas *cp;
	int line_no;

	cp = malloc(sizeof(struct canvas));
	if (NULL == cp) return NULL;

	cp->width = width;
	cp->height = height;
	cp->type = type;

	/* set drawing functions (could almost call them "methods", as they are
	 * (dynamically) tied to the canvas type - that's about as OO as I can
	 * get in C, I suppose) - the idea is that the type of canvas never
	 * changes, so what drawing method to call should be decided once and
	 * for all at creation time. */
	switch(type) {
	case CANVAS_TYPE_RAW:
		cp->draw_hline = raw_canvas_draw_hline;
		cp->draw_vline = raw_canvas_draw_vline;
		cp->draw_upper_corner = raw_canvas_draw_upper_corner;
		cp->draw_lower_corner = raw_canvas_draw_lower_corner;
		cp->write = raw_canvas_write;
		cp->dump = raw_canvas_dump;
		break;
	case CANVAS_TYPE_VT100:
		cp->draw_hline = vt100_canvas_draw_hline;
		cp->draw_vline = vt100_canvas_draw_vline;
		cp->draw_upper_corner = raw_canvas_draw_upper_corner;
		cp->draw_lower_corner = raw_canvas_draw_lower_corner;
		cp->write = vt100_canvas_write;
		cp->dump = vt100_canvas_dump;
		break;
	default:
		assert(0);
	}

	/* allocate space for pointers to lines */
	cp->lines = malloc(height * sizeof(char *));
	if (NULL == cp->lines) return NULL;
	/* now allocate lines */
	for (line_no = 0; line_no < height; line_no++) {
		int c;
		cp->lines[line_no] = malloc((width+1) * sizeof(char));
		if (NULL == cp->lines[line_no]) return NULL;
		for (c = 0; c < width; c++)
			cp->lines[line_no][c] = ' ';
		cp->lines[line_no][c] = '\0';
	}

	return cp;
}

struct canvas *create_raw_canvas(int width, int height)
{
	return create_canvas(width, height, CANVAS_TYPE_RAW);
}

struct canvas *create_vt100_canvas(int width, int height)
{
	return create_canvas(width, height, CANVAS_TYPE_VT100);
}

/* I use accessors so I can keep the structure private */

int get_canvas_width(struct canvas *canvas) { return canvas->width; }

int get_canvas_height(struct canvas *canvas) { return canvas->height; }

char get_canvas_char_at(struct canvas *canvas, int line, int col) { return canvas->lines[line][col]; }

void set_canvas_char_at(struct canvas *canvas, int line, int col, char c) { canvas->lines[line][col] = c; }

char* _get_canvas_line(struct canvas *canvas, int num)
{
	return canvas->lines[num];
}

static void raw_canvas_draw_hline(struct canvas *canvasp, int line, int start_col, int stop_col)
{
	int col;

	assert(line >= 0);
	assert(line < canvasp->height);
	assert(start_col >= 0);
	assert(stop_col < canvasp->width);

	/* <= is intentional (stop position is included in line) */
	for (col = start_col; col <= stop_col; col++) 
		if ('|' == canvasp->lines[line][col])
			canvasp->lines[line][col] = '+';
		else
			canvasp->lines[line][col] = '-';
}

static void vt100_canvas_draw_hline(struct canvas *canvasp, int line, int start_col, int stop_col)
{
	int col;

	assert(line >= 0);
	assert(line < canvasp->height);
	assert(start_col >= 0);
	assert(stop_col < canvasp->width);

	/* <= is intentional (stop position is included in line) */
	for (col = start_col; col <= stop_col; col++) 
		canvasp->lines[line][col] = 'q';
}

/* This only slightly hides implementation details, but there is nothing to
 * prevent the user from directly calling canvas->draw_hline. At least it
 * allows client code to run unchanged, which is a plus. */

void canvas_draw_hline(struct canvas *canvasp, int line, int start_col, int stop_col)
{
	canvasp->draw_hline(canvasp, line, start_col, stop_col);
}

static void raw_canvas_draw_vline(struct canvas *canvasp, int col, int start_line, int stop_line)
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

static void vt100_canvas_draw_vline(struct canvas *canvasp, int col, int start_line, int stop_line)
{
	int line;

	assert(col >= 0);
	assert(start_line >= 0);
	assert(stop_line >= 0);

	/* <= is intentional (stop position is included in line) */
	for (line = start_line; line <= stop_line; line++)
		canvasp->lines[line][col] = 'x';
}

/* See remark about canvas_draw_hline() */

void canvas_draw_vline(struct canvas *canvasp, int col, int start_line, int stop_line)
{
	canvasp->draw_vline(canvasp, col, start_line, stop_line);
}

static void raw_canvas_draw_upper_corner(struct canvas *canvasp, int col, int line, char symbol)
{
	canvas_write(canvasp, col, line, symbol);
}

static void vt100_canvas_draw_upper_corner(struct canvas *canvasp, int col, int line, char symbol)
{
	canvas_write(canvasp, col, line, 'l');
}

void canvas_draw_upper_corner(struct canvas *canvasp, int col, int line, char symbol)
{
	canvasp->draw_upper_corner(canvasp, col, line, symbol);
}

static void raw_canvas_draw_lower_corner(struct canvas *canvasp, int col, int line, char symbol)
{
	canvas_write(canvasp, col, line, symbol);
}

static void vt100_canvas_draw_lower_corner(struct canvas *canvasp, int col, int line, char symbol)
{
	canvas_write(canvasp, col, line, 'm');
}

void canvas_draw_lower_corner(struct canvas *canvasp, int col, int line, char symbol)
{
	canvasp->draw_lower_corner(canvasp, col, line, symbol);
}

static void raw_canvas_write(struct canvas *canvasp, int col, int line, char *text)
{
	assert(col >= 0);
	assert(col < canvasp->width);
	int text_length = strlen(text);
	int space_left = canvasp->width - col;
	int min = text_length < space_left ? text_length : space_left;
	char *dest = canvasp->lines[line]+col;
	strncpy(dest, text, min);
}

static void vt100_canvas_write(struct canvas *canvasp, int col, int line, char *text)
{
	assert(col >= 0);
	assert(col < canvasp->width);
	int text_length = strlen(text);
	int space_left = canvasp->width - col;
	int min = text_length < space_left ? text_length : space_left;
	char *dest = canvasp->lines[line]+col;
	strncpy(dest, text, min);
}

void canvas_write(struct canvas *canvasp, int col, int line, char *text)
{
	canvasp->write(canvasp, col, line, text);
}

// TODO: not sure we need this anymore
enum plus_type find_plus_type(struct canvas *canvasp, int line_nb, int col_nb)
{
	char c_above, c_below, c_before, c_after;
	
	c_above  = 	(0 == line_nb) ?  ' ' 			:  canvasp->lines[line_nb-1][col_nb];
	c_below  = 	(canvasp->height - 1 == line_nb) ? ' ' 	:  canvasp->lines[line_nb+1][col_nb];
	c_before = 	(0 == col_nb) ?  ' ' 			:  canvasp->lines[line_nb][col_nb-1];
	c_after  = 	(canvasp->width - 1 == col_nb) ?  ' ' 	:  canvasp->lines[line_nb][col_nb+1];

	char plus_code[5] = { c_above, c_below, c_before, c_after, '\0'};
	printf("l%d c%d - %s\n", line_nb, col_nb, plus_code);

	/* e.g.   |
	 *       -+- -> "||--"   ;   +-  -> " | -"   
	 *        |                  |
	 */

	if 	(0 == strcmp(" | -", plus_code))
		return UPPER_ANGLE;
	else if (0 == strcmp("|  -", plus_code))
		return LOWER_ANGLE;
	else if (0 == strcmp("||--", plus_code))
		return CROSS;
	else if (0 == strcmp("||- ", plus_code))
		return TEE;
	else
		return UNKNOWN;
}

// TODO: not sure we need this anymore
void canvas_translate_pluses(struct canvas *canvasp)
{
	int line_nb;

	for (line_nb = 0; line_nb < canvasp->height; line_nb++) {
		char *line = canvasp->lines[line_nb];
		int col_nb;
		char c;
		for (col_nb = 0; col_nb < canvasp->width; col_nb++) {
			c = line[col_nb];
			if ('+' == c) {
				enum plus_type type = find_plus_type(canvasp, line_nb, col_nb);
				switch (type) {
				case UPPER_ANGLE:
					line[col_nb] = ',';
					break;
				case LOWER_ANGLE:
					break;
				case CROSS:
					break;
				case TEE:
					break;
				default:
					assert(false);
				}

			}
		}
	}
}

static void raw_canvas_dump(struct canvas* canvasp)
{
	int line;

	for (line = 0; line < canvasp->height; line++)
		printf("%s\n", canvasp->lines[line]);
}

static void vt100_canvas_dump(struct canvas* canvasp)
{
	int line;

	for (line = 0; line < canvasp->height; line++) {
		printf("%s", VT_BEG);
		printf("%s", canvasp->lines[line]);
		printf("%s\n", VT_END);
	}
}

void canvas_dump(struct canvas *canvasp) 
{
	canvasp->dump(canvasp);
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
