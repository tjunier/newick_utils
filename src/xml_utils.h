/* Utility functions for XML */

/* Returns a string with a version of the argument string with &, <, >, etc
 * replaced by &amp;, &lt;, etc. Argument is not modified; space is allocated,
 * or NULL is returned if no space could be allocated.  * */

char *escape_predefined_character_entities(const char*);

