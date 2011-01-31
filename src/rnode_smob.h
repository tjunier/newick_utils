
#include "rnode.h"

/* Initializes the SCM rnode SMOB */
void init_scm_rnode();

/* Creates an rnode SMOB. Argument is a pointer to a struct rnode. */
SCM rnode_smob(struct rnode*);

/* Accessors */

/* All the following f()s work as getters if the second argument is 
 * unspecified, and as setters otherwise */

SCM rnode_smob_label(SCM rnode_smob, SCM label);
