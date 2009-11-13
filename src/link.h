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

#define RM_CHILD_IS_ROOT (-1)
#define RM_CHILD_MEM_ERROR (-2)

/* NOTE: Tree properties such as 'nodes_in_order' will generally be invalidated
 * by these methods. You will need either to recompute them, or to avoid
 * relying on them after the call. For example, it is safe to splice out a node
 * and then call to_newick() on the tree's root. On the other hand, the list of
 * nodes in the tree structure will still contain the spliced-out node (which
 * is good anyway since you need it to free the spliced-out node).*/

enum unlink_rnode_status { UNLINK_RNODE_DONE, UNLINK_RNODE_ROOT_CHILD,
	UNLINK_RNODE_ERROR };

struct rnode;

extern struct rnode * unlink_rnode_root_child;

/* Adds 'child' to 'parent''s list of children nodes */
/* Returns FAILURE iff there was a problem (malloc(), most probably) */
int add_child(struct rnode *parent, struct rnode *child);

/* sets 'edge' as 'child''s parent edge, and 'child' as 'edge''s child
 * node. */

// OBSOLETE
// void set_parent_edge(struct rnode *child, struct redge *edge);

// OBSOLETE - use add_child() 
// void link_p2c(struct rnode *parent, struct rnode *child, char *length);

/* Inserts a node with the label 'label' in this node's parent edge. The two
 * resulting edges will each have a length of half the original's, if
 * specified. */
/* Returns SUCCESS unless the new node can't be created (malloc()...) */

int insert_node_above(struct rnode *node, char *label);

/* Replaces child node 'old' by 'new'. Does nothing if 'old' is not found. */

void replace_child(struct rnode *node, struct rnode *old, struct rnode *new);

/* Splices out node 'node' (which must be inner). Children are linked directly
 * to 'node''s parent. New edges have edges such that the children nodes'
 * depth is unchanged. */

/* Should NOT be used for deleting ("pruning") nodes (will segfault)! Use
 * unlink_rnode() instead. */
/* Returns FAILURE iff there is a malloc() problem. */

int splice_out_rnode(struct rnode *node);

/* deletes child from parent's children list; returns the index of the child in
 * the list prior to removal. Child's parent is set to NULL. In case of error,
 * returns a negative number (see RM_CHILD_*) above. */

int remove_child(struct rnode *child);

/* Inserts child into parent's children list, at the specified index. Sets the
 * child's parent. */
/* Returns FAILURE iff there was a problem (malloc(), most probably) */

int insert_child(struct rnode *parent, struct rnode *child, int index);

/* Swaps 'node' and its parent, i.e, parent becomes a child of 'node' */
/* Returns FAILURE in case of problem (memory) */

int swap_nodes(struct rnode *node);

/* Removes a node from its parent's children list. The node is not freed. There
 * are the following cases: If the removed node had more than one sibling, the
 * tree is in a normal state and the function stops here (case 1). If not, the
 * tree is abnormal because the parent has only one child. There are two
 * possibilities: i) the parent is the root (case 2) - the function stops there
 * and returns the root's (only) child so that it may become the tree's root
 * (we cannot do this here, since we're working at node level, not tree level);
 * ii) the parent is an inner node (case 3) - it gets spliced out.
 * RETURN VALUE:
	UNLINK_RNODE_ERROR - there was an error (malloc(), most probably)
	UNLINK_RNODE_DONE - cases 1 and 3 (nothing else to do)
	UNLINK_RNODE_ROOT_CHILD - case 2; also sets the external variable
		'unlink_rnode_root_child' to the root's child.
*/

int unlink_rnode(struct rnode *);

/* Returns the node's list of siblings. Siblings appear in the same order as in
 * the parent's children list. The list is empty for root, it may be empty for
 * leaves (it's not illegal for a node to have just one child), but this is
 * unusual. */

struct llist *siblings(struct rnode *);
