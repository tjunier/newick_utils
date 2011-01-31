/* Represents an rnode as a SMOB (Guile primitive) */

#include "rnode.h"

static struct rnode_smob {
	struct rnode *c_rnode;
};

static scm_t_bits rnode_tag;

void init_scm_rnode()
{
	rnode_tag = scm_make_smob_type("node", sizeof(struct rnode_smob));
	scm_set_smob_mark(rnode_tag, mark_rnode);
	scm_set_smob_free(rnode_tag, free_rnode);
	scm_set_smob_print(rnode_tag, print_rnode);
}

SCM rnode_smob(struct rnode *node)
{
	SCM smob;
	SCM_NEWSMOB (smob, rnode_tag, node);

	return smob;
}
