/* my asprintf() - a version of sprintf() that allocates storage (which must be
 * free()d, of course). */

char *masprintf(const char *format, ...);

/* like strcat(3), but reallocates storage if needed (which must be freed). */

char *mastrcat(const char *src, const char *dest);
