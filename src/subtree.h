
struct llist;
struct rnode;

enum monophyly { MONOPH_TRUE, MONOPH_FALSE, MONOPH_ERROR };

/* Given a list of nodes ("descendants") and an ancestor node, returns
 * MONOPH_TRUE if all the ancestor's labeled (leaf?) descendant nodes, and no
 * others,  are in the list. Otherwise returns MONOPH_FALSE, or MONOPH_ERROR if
 * there was a memory error.
 * Assumes: descendants contains only nodes, and at least one node. */

enum monophyly is_monophyletic(struct llist *descendants,
		struct rnode *ancestor);
