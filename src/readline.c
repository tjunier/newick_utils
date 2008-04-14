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

	wt->string = string;
	wt->word_start = string;
	wt->word_stop = NULL;

	return wt;
}

char *wt_next(struct word_tokenizer *wt)
{
	size_t sep_len = strspn(wt->word_start, " \t");
	wt->word_start = wt->word_start + sep_len;
	wt->word_stop = strpbrk(wt->word_start, " \t");
	int wlen = wt->word_stop - wt->word_start;
	char *word = malloc((wlen + 1) * sizeof(char));
	if (NULL == word) { perror(NULL); exit(EXIT_FAILURE); }
	strncpy(word, wt->word_start, wlen);

	return word;
}

