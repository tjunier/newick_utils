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
#include <string.h>

#include "readline.h"

char * read_line(FILE *file)
{
	char *line;
	long fpos;
	long len = 0L;
	int c;
	
	/* return NULL if EOF */
	if (feof(file)) return NULL;

	fpos = ftell(file);	/* remember where we start */
	if (-1 == fpos) {
		perror(NULL);
		exit(EXIT_FAILURE);
	}

	/* find next newline */
	while ((c = getc(file)) != EOF) {
		if ('\n' == c)
			break;
		len++;
	}

	/* return NULL if EOF and line length is 0 */
	if (feof(file) && 0 == len) return NULL;

	/* allocate memory for line */
	line = malloc((1 + len) * sizeof(char));
	if (NULL == line) {
		perror(NULL);
		exit(EXIT_FAILURE);
	}

	/* return to where we started */
	if (-1 == fseek(file, fpos, SEEK_SET)) {
		perror(NULL);
		exit(EXIT_FAILURE);
	}

	fgets(line, len+1, file);

	/* consumes newline (otherwise gets stuck here...) */
	fgetc(file);

	return line;
}

struct word_tokenizer *create_word_tokenizer(const char *string)
{
	struct word_tokenizer *wt = malloc(sizeof(struct word_tokenizer));
	if (NULL == wt) { perror(NULL); exit(EXIT_FAILURE); }

	wt->string = strdup(string);
	wt->string_len = strlen(wt->string);
	wt->word_start = wt->string;
	wt->word_stop = NULL;

	return wt;
}

char *wt_next(struct word_tokenizer *wt)
{
	/* No more tokens to parse */
	if (wt->word_start >= wt->string + wt->string_len) {
		return NULL;
	}

	/* Find the start of the next "word" */
	size_t sep_len = strspn(wt->word_start, " \t\n");
	
	wt->word_start = wt->word_start + sep_len;
	/* Find the end of that word. If the word starts with ' or ", look for
	 * the next ' or "; otherwise look for whitespace or NULL. */
	if (*wt->word_start == '\'') {
		wt->word_stop = 1 + strpbrk(wt->word_start + 1, "'");
		if (NULL == wt->word_stop) { wt->word_stop = wt->string + wt->string_len; }
	} else if (*wt->word_start == '"') {
		wt->word_stop = 1 + strpbrk(wt->word_start + 1, "\"");
		if (NULL == wt->word_stop) { wt->word_stop = wt->string + wt->string_len; }
	} else {
		wt->word_stop = strpbrk(wt->word_start, " \t\n");
		if (NULL == wt->word_stop) { wt->word_stop = wt->string + wt->string_len; }
	}
	/* Find the word's length, and allocate memory for it */
	int wlen = wt->word_stop - wt->word_start;
	char *word = malloc((wlen + 1) * sizeof(char));
	if (NULL == word) { perror(NULL); exit(EXIT_FAILURE); }
	/* Copy the word into the allocated space, terminating the string */
	strncpy(word, wt->word_start, wlen);
	word[wlen] = '\0';
	/* Set the next word's start pos to just after the present word's end. */

	wt->word_start = wt->word_stop + 1;

	return word;
}

char *wt_next_noquote(struct word_tokenizer *wt)
{
	char *word = wt_next(wt);
	if (NULL == word) return NULL;

	size_t len = strlen(word);
	if (word[0] == '"' || word[0] == '\'')
		if (word[len-1] == word[0]) {
			char *unquoted = malloc((len) * sizeof(char));
			if (NULL == unquoted) {
				perror(NULL);
				exit(EXIT_FAILURE);
			}
			strncpy(unquoted, word+1, len-2);
			unquoted[len-2] = '\0';
			free(word);
			return unquoted;
		}
	return word;
}

void destroy_word_tokenizer(struct word_tokenizer *wt)
{
	free(wt->string);
	free(wt);
}
