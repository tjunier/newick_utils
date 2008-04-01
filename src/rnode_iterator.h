/* functions for iterating over a rnode hierarchy given its root */

struct rnode;
struct rnode_iterator;

/* Creates an iterator. You can then pass it to the functions below. */

struct rnode_iterator *create_rnode_iterator(struct rnode *root);

/* Destroys the iterator, freeing allocated memory */

void destroy_rnode_iterator(struct rnode_iterator *);

/* Gets the next node information */

struct rnode *rnode_iterator_next(struct rnode_iterator *);
