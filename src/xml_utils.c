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
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

char *escape_predefined_character_entities(const char *string)
{
	const int MAX_ESCAPE_LEN = 6;	/* e.g. "&quot;" is 6 chars long */

	char *s = strdup(string); /* local copy: don't touch the const
				     argument */

	/* This allocates for enough characters even if all chars in the
	 * argument have to be escaped, and all of them have the longest
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

/* Cygwin's realloc() chokes on this, I'm not sure why. I understand that it is
 * dangerous to ignore the return value (as the data may have been moved),
 * but I do update the 'return' pointer to the new value. Oh well, we'll have
 * go with a little excess memory under Cygwin, unless I find out what's going
 * on here. */

#ifndef __CYGWIN__
	size_t new_length = strlen(result) + 1;
	result = realloc(result, new_length); /* NULL handled by caller */
#endif
	return result;
}
