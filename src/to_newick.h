/* Returns a Newick string representing the tree rooted at 'node'.*/

struct rnode;

/* Returns a Newick representation of the tree. Memory is allocated, don't
 * forget to free() it. */

char *to_newick(struct rnode* node);
