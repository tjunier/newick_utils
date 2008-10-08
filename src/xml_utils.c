#include <string.h>
#include <stdlib.h>
#include <stdio.h>

char *escape_predefined_character_entities(const char *string)
{
	const int MAX_ESCAPE_LEN = 6;	/* e.g. "&quot;" is 6 chars long */

	char *s = strdup(string); /* local copy: don't touch the const
				     argument */

	/* This allocates for enough characters even if all chars in the
	 * argument ahve to be escaped, and all of them have the longest
	 * possible escape  - like this we're absolutely sure our escaped
	 * string will fit. And don't worry, we're going to free the unused
	 * memory. */
	char *result = malloc(MAX_ESCAPE_LEN * strlen(string) * sizeof(char));
	if (NULL == result) 
		return NULL;

	char *c;			/* scans s */
	char *result_end = result;	/* points to last char copied */
	for (c = s; *c != '\0'; c++) {
		switch (*c) {
		case '&':
			strncpy(result_end, "&amp;", 5);
			result_end += 5;
			break;
		case '<':
			strncpy(result_end, "&lt;", 4);
			result_end += 4;
			break;
		case '>':
			strncpy(result_end, "&gt;", 4);
			result_end += 4;
			break;
		case '\'':
			strncpy(result_end, "&apos;", 6);
			result_end += 6;
			break;
		case '"':
			strncpy(result_end, "&quot;", 6);
			result_end += 6;
			break;
		default:
			*result_end = *c;
			result_end++;
			break;
		}
	}
	*result_end = '\0';
	free(s);

	size_t new_length = strlen(result) + 1;
	result = realloc(result, new_length);

	return result;
}
