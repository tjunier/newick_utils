
struct rooted_tree;

/* Parses a tree from nwsin, returns a pointer to a tree structure, or NULL if
 * there is no input. It is the caller's responsibility to set nwsin (which by
 * default is stdin). */

struct rooted_tree *parse_tree();
