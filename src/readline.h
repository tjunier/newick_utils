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
#include <stdio.h>
#include <stdbool.h>

enum read_status { READLINE_EOF, READLINE_ERROR };

extern enum read_status read_line_status;

enum next_status { NEXT_TOKEN_END, NEXT_TOKEN_ERROR };

extern enum next_status next_token_status;

/* Returns a line from a file, as a pointer to an allocated buffer. Returns
 * NULL if EOF or error (the external variable 'read_line_status' will be set
 * so that callers can tell EOF from errors). The buffer should be free()d when
 * no longer needed. */

char *read_line(FILE *);

/* Creates a word tokenizer for a string, passed as arguments. Function
 * wt_next() returns tokens. */
/* Returns NULL if the structure can't be created (malloc() error) */

struct word_tokenizer *create_word_tokenizer(const char *);

/* Returns the next token, using whitespace as token separators. Allocates
 * space for the token, which must e free()d later.  Quote-delimited strings
 * (single and double quotes) are honored, i.e. 'two words' is a single token,
 * and the quotes are retained (because they are valid in Newick labels). */
/* Returns NULL when there is no more token, or in case of error. The external
 * variable 'next_token_status' will be set so that the caller can know what
 * caused NULL. */

char *wt_next(struct word_tokenizer *);

/* Same as above, but removes any leading or trailing quotes (' or ") */

char *wt_next_noquote(struct word_tokenizer *);

/* Frees a word_tokenizer */

void destroy_word_tokenizer(struct word_tokenizer *);

/* Returns true IFF line only contains whitespace (as per isspace(), see
 * isalpha(3)), or if line is empty. Useful when you need to ignore such
 * lines. */

bool is_all_whitespace(char *line);
