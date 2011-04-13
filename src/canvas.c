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
