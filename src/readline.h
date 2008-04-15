#include <stdio.h>

struct word_tokenizer {
	char *string;
	char *word_start;
	char *word_stop;
	size_t string_len;
};

/* Returns a line from a file, as a pointer to an allocated buffer. Returns
 * NULL if EOF. The buffer should be free()d when no longer needed. */

char *read_line(FILE *);

/* Creates a word tokenizer for a string, passed as arguments. Function wt_next() returns tokens. */

struct word_tokenizer *create_word_tokenizer(const char *);

/* Returns the next token, using whitespace as token separators. Allocates
 * space for the token, which must e free()d later.  Quote-delimited strings
 * (single and double quotes) are honored, i.e. 'two words' is a single token,
 * and the quotes are retained (because they are valid in Newick labels). */

char *wt_next(struct word_tokenizer *);

/* Frees a word_tokenizer */

void destroy_word_tokenizer(struct word_tokenizer *);
