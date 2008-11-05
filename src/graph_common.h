struct rooted_tree;

/* Replaces '_' by ' ' (changes argument!) */

void underscores2spaces(char *);

/* Removes (single) quotes (changes argument!) */

void remove_quotes(char *);

/* Prettify the labels by changing underscores to spaces and removing quotes.
 * ARgument is changed. Do not call this before printing out Newick, or the
 * labels will be invalid. */

void prettify_labels(struct rooted_tree *);
