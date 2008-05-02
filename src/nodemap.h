/* Functions for creating and searching label->node maps */

struct hash;
struct llist;

/* Given a list of nodes, creates a map of all nodes, keyed by label. Empty
 * labels are ignored. Labels should be unique. Redundant labels will not
 * cause a crash, but may cause unexpected results as the "map" will not be
 * one-to-one. */

struct hash *create_label2node_map(const struct llist *);
