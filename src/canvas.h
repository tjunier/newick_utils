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


/**
 * \file canvas.h Functions for creating and using text canvases.
 */

/**
 * Creates a canvas of w chars by h lines. Positions on the canvas start at 0
 * (i.e., C-style).  Returns a (pointer to a) canvas, or NULL iff canvas can't
 * be allocated. Once no longer needed, the canvas should be freed with
 * destroy_canvas().
 * \param w the number of columns
 * \param l the number of lines (rows)
 * \return a pointer to a struct canvas
 */ 

struct canvas *create_canvas(int w, int l);

/** Returns the canvas' width in columns
 *
 * \param	canvas	a pointer to the canvas
 * \return	its width
 */

int get_canvas_width(struct canvas *canvas);

/** Returns the canvas' height in rows
 *
 * \param	canvas	a pointer to the canvas
 * \return	its height
 */

int get_canvas_height(struct canvas *canvas);

/** Returns one line from the canvas. Meant to be used in tests, mostly (hence
 * the _ prefix). The line is NOT copied, so do not call free on this unless
 * you know what you're
 * doing!
 *
 * \param	canvas	a pointer to the canvas
 * \param	num	what line number to get
 * \return	the line
 */

char* _get_canvas_line(struct canvas *canvas, int num);

/** Draws a vertical line on the canvas.
 * The line is on column \a col form line \a start to
 * line \a end. The line is made of '\c |', except if there already is a
 * '\c -', in which case a '\c +' is written.
 * \param[out]	canvas 	the canvas to draw on
 * \param	col	the column to draw at
 * \param	start	the first line
 * \param	end	the last line */

void canvas_draw_vline(struct canvas* canvas, int col, int start, int end);

/** Draws a horizontal line on the canvas. The line is on line \a line, and
 * goes from column \a start to column \a end (inclusive). The line is made
 * of '\c -', except if there already is a '\c |', in which case a '\c +' is
 * written instead. 
 * \param[out] 	canvas 	the canvas to draw on
 * \param 	line	the line to draw at
 * \param	start	the first column
 * \param	end	the last column */

void canvas_draw_hline(struct canvas* canvas , int line, int start, int end);

/** Writes a text string.
 * \param[out]	canvas	the canvas to write the string on
 * \param	col	the colum of the first character in the string
 * \param	line	the line the string is written at
 * \param	text	the string to write (NULL-terminated) */

void canvas_write(struct canvas* canvas, int col, int line, char *text);

/** Dumps the canvas to standard output. Use this function after filling the
 * canvas' contents with the draw and write functions.
 * \param[out]	canvas * the canvas to dump. */

void canvas_dump(struct canvas* canvas);

/** (Debugging function) like canvas_dump(), but prints line numbers. */

void canvas_inspect(struct canvas* canvasp);

/** Releases all memory held by the canvas. Don't use it after this! */

void destroy_canvas(struct canvas*);
