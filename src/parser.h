
struct rooted_tree;

/* Parses a tree from yyin, returns a pointer to a tree structure, or NULL if
 * there is no input. It is the caller's responsibility to set yyin (which by
 * default is stdin). */

struct rooted_tree *parse_tree();
