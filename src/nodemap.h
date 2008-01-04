/* Functions for creating and searching label->node maps */

struct node_map;
struct llist;

/* Given a list of nodes, creates a map of all nodes, keyed by label. */

struct node_map *create_node_map(const struct llist *);

/* Given a label, return the node which has the label - if the label is not
 * unique, it cannot be predicted which node will be returned. */

struct rnode* get_node_with_label(const struct node_map *, const char *);

/* A debugging function, just lists the nodes in the map. */

void dump_map(const struct node_map *);
