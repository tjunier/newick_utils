/* Functions for creating and searching label->node maps */

struct hash;
struct llist;

/* Given a list of nodes (e.g., tree->nodes_in_order), creates a map of all
 * nodes, keyed by label. Empty labels are ignored. Labels should be unique.
 * Redundant labels will not cause a crash, but may cause unexpected results as
 * the "map" will not be one-to-one. */

struct hash *create_label2node_map(const struct llist *);

/* Given a list of nodes (e.g., tree->nodes_in_order), creates a map of _lists_
 * of nodes of the same label - this means we can handle tree with nonunique
 * labels (cf create_label2node_map() which assumes labels are unique). Empty
 * labels are treated like any other label. Nodes of the same label are stored
 * in the order they are in the parameter list. */

struct hash *create_label2node_list_map(const struct llist *);

/* Destroys a label->node list map such as those created by
 * create_label2node_list_map() */

void destroy_label2node_list_map(struct hash *);
