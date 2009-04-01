/* my asprintf() - a version of sprintf() that allocates storage (which must be
 * free()d, of course). */

char *masprintf(const char *format, ...);

/* like strcat(3), but reallocates storage if needed (which must be freed). */
/* Not sire we'll keep this one - I wrote it when I wanted to put all multiple SVG trees in a single file, which is no longer how things are being done. Still, it might come in handy. */

char *mastrcat(const char *src, const char *dest);
