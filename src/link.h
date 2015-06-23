/* 

Copyright (c) 2009 Thomas Junier and Evgeny Zdobnov, University of Geneva
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice,
    this list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright notice,
    this list of conditions and the following disclaimer in the documentation
    and/or other materials provided with the distribution.
* Neither the name of the University of Geneva nor the names of its
    contributors may be used to endorse or promote products derived from this
    software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/
/* Functions for creating and manipulating trees at the node level. */

/* remove_child() returns negative values on error (and only then). We start
 * at -10, that should allow for enough kinds of errors... */
enum remove_child_status {  RM_CHILD_HAS_NO_PARENT = -10, RM_CHILD_MEM_ERROR };

const static int DICHOT_NO_OP;
const static int DICHOT_NO_MEM;

/* NOTE: Tree properties such as 'nodes_in_order' will generally be invalidated
 * by these methods. You will need either to recompute them, or to avoid
 * relying on them after the call. For example, it is safe to splice out a node
 * and then call to_newick() on the tree's root. On the other hand, the list of
 * nodes in the tree structure will still contain the spliced-out node (which
 * is good anyway since you need it to free the spliced-out node).*/

enum unlink_rnode_status { UNLINK_RNODE_DONE, UNLINK_RNODE_ROOT,
	UNLINK_RNODE_ROOT_CHILD, UNLINK_RNODE_ERROR };

struct rnode;

/* Adds 'child' to 'parent''s children (after last child). Added child's
 * next_sibling is LEFT UNTOUCHED, so don't count on it being NULL. */

void add_child(struct rnode *parent, struct rnode *child);

/* sets 'edge' as 'child''s parent edge, and 'child' as 'edge''s child
 * node. */

/* Inserts a node with the label 'label' in this node's parent edge. The two
 * resulting edges will each have a length of half the original's, if
 * specified. */
/* Returns SUCCESS unless the new node can't be created (malloc()...) */

int insert_node_above(struct rnode *node, char *label);

/* Replaces child node 'old' by 'new'. Does nothing if 'old' is not found. */

void replace_child(struct rnode *old, struct rnode *new);

/* Splices out node 'node' (which must be inner). Children are linked directly
 * to 'node''s parent. New edges have edges such that the children nodes'
 * depth is unchanged. */

/* Should NOT be used for deleting ("pruning") nodes (will segfault)! Use
 * unlink_rnode() instead. */
/* Returns FAILURE iff there is a malloc() problem. */

int splice_out_rnode(struct rnode *node);

/* deletes child from parent's children list; returns the index of the child in
 * the list prior to removal. Child's parent is set to NULL. In case of error,
 * returns a negative number (see RM_CHILD_*) above. This function makes no
 * attempt to leave the tree in a coherent state, see unlink_rnode() for this.
 * */

enum remove_child_status remove_child(struct rnode *child);

/* Inserts 'insert' into 'parent's children list, at the specified index. Sets
 * the child's parent. An index of 0 means insertion before the first child, an
 * index of 1 means after the first child, etc. If there are n children, an
 * index of n means inserttion after the last child. */
/* Returns FAILURE iff index is off limits (<0 or > number of children). */

int insert_child(struct rnode *parent, struct rnode *insert, int index);

/* Swaps 'node' and its parent which must be the root, i.e, parent becomes a
 * child of 'node', which itself becomes the new root. */
/* Returns FAILURE in case of memory problem. Assumes that parent is root. */

int swap_nodes(struct rnode *node);

/* As above, but if 'i_node_lbl_as_support' is true, then inner node labels are
 * treated as branch properties and swapped as well (i.e., migrate from child
 * to parent). This is used e.g. when rerooting bootstrapped trees. */

int swap_nodes_wsupport(struct rnode *node, bool i_node_lbl_as_support);

/* Removes a node from its parent's children list. The node is not freed. There
 * are the following cases: If the removed node had more than one sibling, the
 * tree is in a normal state and the function stops here (case 1). If not, the
 * tree is abnormal because the parent now has only one child. There are two
 * possibilities: i) the parent is the root (case 2) - the function stops there
 * and sets an external variable to the root's (only) child so that it may
 * become the tree's root (we cannot do this here, since we're working at node
 * level, not tree level) - function get_unlink_rnode_root_child() can be used
 * to retrieve it; ii) the parent is an inner node (case 3) - it gets spliced
 * out.
 * RETURN VALUE:
	UNLINK_RNODE_ERROR - there was an error (malloc(), most probably)
	UNLINK_RNODE_DONE - cases 1 and 3 (nothing else to do)
	UNLINK_RNODE_ROOT_CHILD - case 2; use get_unlink_rnode_root_child()
		to get the root's only child.
*/

int unlink_rnode(struct rnode *);
struct rnode *get_unlink_rnode_root_child();

/* Returns the node's list of siblings (NOT including the argument node).
 * Siblings appear in the same order as in the parent's children list. The list
 * is empty for root, it may be empty for leaves (it's not illegal for a node
 * to have just one child), but this is unusual. */

struct llist *siblings(struct rnode *);

/* Removes all children (does not free them). Has no effect is node is a leaf.
 * */ 

void remove_children(struct rnode *);

/* takes 2 lengths (as strings), and return their sum (as a string) */

char *add_len_strings(char *ls1, char *ls2);

/* does an elementary dichotomization operation: takes the next and second-next
 * siblings of the argument node, makes a new node with these two as children,
 * and adds that new node as sibling to the argument node. Not meant to be
 * called outside dichotomize() (hence the leading _), but visible so it can be
 * tested. Meant to be called on the antepenultimate (next-to-next-to-last, that
 * is :-) child. Example: given (A,B,C,D,E), call on C and get (A,B,C,(D,E)).
 * Further calls on B then A yield (A,(B,(C,(D,E)))). */

struct rnode* _dichotomize_next_two_siblings(struct rnode*);

/* apply the above primitive to the children of a node, so that they are
 * entirely dichotomized: called on (A,B,C,D,E), this yields (A,(B,(C,(D,E)))).
 * */

int dichotomize_children(struct rnode*);
