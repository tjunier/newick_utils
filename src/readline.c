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

void destroy_word_tokenizer(struct word_tokenizer *wt)
{
	free(wt->string);
	free(wt);
}
