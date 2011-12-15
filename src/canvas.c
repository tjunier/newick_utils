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

enum plus_type { UPPER_ANGLE, LOWER_ANGLE, CROSS, TEE, UNKNOWN };

static const char *VT_BEG = "\033(0\017";
static const char *VT_END = "\033(B";

struct canvas {
	int width;
	int height;
	char **lines;
	};

struct canvas *create_canvas(int width, int height)
{
	struct canvas *cp;
	int line_no;

	cp = malloc(sizeof(struct canvas));
	if (NULL == cp) return NULL;

	cp->width = width;
	cp->height = height;
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

/* I use getters so I can keep the structure private */

int get_canvas_width(struct canvas *canvas) { return canvas->width; }

int get_canvas_height(struct canvas *canvas) { return canvas->height; }

char get_canvas_char_at(struct canvas *canvas, int line, int col) { return canvas->lines[line][col]; }

void set_canvas_char_at(struct canvas *canvas, int line, int col, char c) { canvas->lines[line][col] = c; }

char* _get_canvas_line(struct canvas *canvas, int num)
{
	return canvas->lines[num];
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
	assert(col >= 0);
	assert(col < canvasp->width);
	int text_length = strlen(text);
	int space_left = canvasp->width - col;
	int min = text_length < space_left ? text_length : space_left;
	char *dest = canvasp->lines[line]+col;
	strncpy(dest, text, min);
}

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

void canvas_dump(struct canvas* canvasp)
{
	int line;

	for (line = 0; line < canvasp->height; line++)
		printf("%s\n", canvasp->lines[line]);
}

void canvas_dump_vt100(struct canvas* canvasp)
{
	int line;

	for (line = 0; line < canvasp->height; line++) {
		char *c;
		for (c = canvasp->lines[line]; '\0' != *c; c++)
			switch (*c) {
			case '|':
				printf("%sx%s", VT_BEG, VT_END);
				break;
			case '-':
				printf("%sq%s", VT_BEG, VT_END);
				break;
			case '/':
				printf("%sl%s", VT_BEG, VT_END);
				break;
			case '\\':
				printf("%sm%s", VT_BEG, VT_END);
				break;
			case '*':
				printf("%sn%s", VT_BEG, VT_END);
				break;
			case '#':
				printf("%st%s", VT_BEG, VT_END);
				break;
			case '+':
				printf("%su%s", VT_BEG, VT_END);
				break;
			default:
				putchar(*c);
				break;
			}
		putchar('\n');
	}
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
