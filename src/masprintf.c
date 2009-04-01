/* Slavishly pasted from the printf(3) man page and only trivially adapted. */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

char * masprintf(const char *fmt, ...)
{
	/* Guess we need no more than 100 bytes - in fact we could use 0 and
	 * let vsnprintf() tell us... */
	int n, size = 100;
	char *p, *np;
	va_list ap;

	if ((p = malloc(size)) == NULL)
		return NULL;

	while (1) {
		/* Try to print in the allocated space. */
		va_start(ap, fmt);
		n = vsnprintf(p, size, fmt, ap);
		va_end(ap);
		/* If that worked, return the string. */
		if (n > -1 && n < size)
			return p;
		/* Else try again with more space. */
		if (n > -1)    /* glibc 2.1 */
			size = n+1; /* precisely what is needed */
		else           /* glibc 2.0 */
			size *= 2;  /* twice the old size */
		if ((np = realloc (p, size)) == NULL) {
			free(p);
			return NULL;
		   } else {
		       p = np;
		   }
	}
}

char * mastrcat(char *dest, const char *src)
{
	size_t dest_len = strlen(dest);
	size_t src_len = strlen(src);
	
	/* 'dest' MUST have been allocated by malloc() etc */
	char * result = realloc(dest, dest_len + src_len + 1);
	if (NULL == result) { perror(NULL); exit(EXIT_FAILURE); }

	strncpy(result + dest_len, src, src_len + 1);

	return result;
}

