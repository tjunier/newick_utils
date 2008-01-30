
/* concatenates two strings, allocates memory. Returns NULL if malloc() fails.
 * */

char *concat(char *s1, char *s2);

/* Appends s2 to s1. Returns a pointer to the concatenated string. Uses
 * realloc(), so does not allocate new (net) memory for s1. */

char *append_to(char *s1, char *s2);
