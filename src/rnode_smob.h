
struct rnode;

/* Initializes the SCM rnode SMOB */
void init_scm_rnode();

/* Creates an rnode SMOB. Argument is a pointer to a struct rnode. */
SCM rnode_smob(struct rnode*);

/* Accessors */

/* All the following f()s work as getters if the second argument is 
 * unspecified, and as setters otherwise */

SCM rnode_smob_label(SCM rnode_smob, SCM label);

/* Other Functions */

/* Returns the argument's parent node, as a SMOB */

SCM rnode_smob_parent(SCM rnode_smob);

/* Returns the argument's first child, or undefined if it is a leaf. */

SCM rnode_smob_first_child(SCM rnode_smob);

/* Returns the argument's last child, or undefined if it is a leaf. */

SCM rnode_smob_last_child(SCM rnode_smob);

/* Returns the argument's number of children */

SCM rnode_smob_children_count(SCM rnode_smob);

/* Dumps the subtree rooted at the argument rnode. Returns undefined. */

SCM rnode_smob_dump_subclade(SCM rnode_smob);

/* Returns a list of all the argument node's children (empty iff leaf) */

SCM rnode_smob_children_list(SCM rnode_smob);
