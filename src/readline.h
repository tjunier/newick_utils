#include <stdio.h>

/* Returns a line from a file, as a pointer to an allocated buffer. Returns
 * NULL if EOF. The buffer should be free()d when no longer needed. */

char *read_line(FILE *);
