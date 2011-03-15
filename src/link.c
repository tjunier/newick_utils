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

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>

#include "rnode.h"
#include "list.h"
#include "link.h"
#include "masprintf.h"
#include "common.h"

/* Avoid global variables by making external vars static and using a getter. */

static struct rnode *unlink_rnode_root_child;

struct rnode *get_unlink_rnode_root_child()
{
	return unlink_rnode_root_child;
}

void add_child(struct rnode *parent, struct rnode *child)
{
	child->parent = parent;

	if (0 == parent->child_count)
		parent->first_child = child;
	else
		parent->last_child->next_sibling = child;

	parent->child_count++;
	parent->last_child = child;
}

/* Returns half the length passed as a parameter (as char *), or "". */

char * compute_new_edge_length(char * length_as_string)
{
	char *result;

	if (0 != strcmp("", length_as_string)) {
		double length = atof(length_as_string);
		result = masprintf("%g", length / 2);
		if (NULL == result) return NULL;
	} else  {
                /* Note: we cannot just say result = ""; because result will be
                 * free()d later, so it has to have been dynamically allocated.
                 * */
                result = strdup("");
        }

	return result;
}

int insert_node_above(struct rnode *this, char *label)
{
	struct rnode *parent;
	struct rnode *new;
	char * new_edge_length;	/* both new edges have 1/2 the length of the
				   old one */

	/* find parent edge and parent node */
	parent = this->parent;
	new_edge_length = compute_new_edge_length(this->edge_length_as_string);
	if (NULL == new_edge_length) return FAILURE;
	/* create new node */
	new = create_rnode(label, new_edge_length);
	if (NULL == new) return FAILURE;
	free(this->edge_length_as_string);
	this->edge_length_as_string = strdup(new_edge_length);
	replace_child(this, new);
	this->next_sibling = NULL;
	/* link new node to this node */
	add_child(new, this);

	free(new_edge_length);

	return SUCCESS;
}
	
void replace_child (struct rnode *old, struct rnode *new)
{
	/* To replace the old rnode by the new one, we need a pointer to the
	 * node _before_ the old node (so as to be able to change its
	 * next_sibling member). We therefore start "ahead" of the list, using
	 * a dummy first child. */

	struct rnode dummy_first;	/* NOT a pointer! */
	struct rnode *current;
	struct rnode *dad = old->parent;

	dummy_first.next_sibling = dad->first_child;

	for (current = &dummy_first; NULL != current->next_sibling;
			current = current->next_sibling) 
		if (current->next_sibling == old) 
			break;

	current->next_sibling = new;
	new->next_sibling = old->next_sibling; 
	if (dad->first_child == old) 
		dad->first_child = new;
	if (dad->last_child == old)
		dad->last_child = new;
	new->parent = dad;
}

char *add_len_strings(char *ls1, char *ls2)
{
	char * result;

	/* if ls1 and ls2 are not both "" */
	if (	strcmp("", ls1) != 0 ||
		strcmp("", ls2) != 0)	{
		double l1 = atof(ls1);
		double l2 = atof(ls2);
		result = masprintf("%g", l1 + l2);
		if (NULL == result) return NULL;
	} else {
		result = strdup("");
		/* I'd be REALLY suprised if this fails... */
		if (NULL == result) return NULL;
	}

	return result;
}

/* 'this' node is the one that is to be spliced out. All nodes and edges are
 * relative to this one. */

int splice_out_rnode(struct rnode *this)
{
	struct rnode dummy_head;	/* see [where?] */
	struct rnode *parent = this->parent;
	struct rnode *current_child, *current_sibling;

	/* change the children's parent edges: they must now point to their
	 * 'grandparent', and the length from their parent to their grandparent
	 * must be added. */
	for (current_child = this->first_child; NULL != current_child;
			current_child = current_child->next_sibling) {
		char *new_edge_len_s = add_len_strings(
			this->edge_length_as_string,
			current_child->edge_length_as_string);
		if (NULL == new_edge_len_s) return FAILURE;
		free(current_child->edge_length_as_string);
		current_child->edge_length_as_string = new_edge_len_s;
		current_child->parent = parent;  /* instead of this node */
	}

	/* Insert this node's children in place of itself among its parent's
	 * children */
	dummy_head.next_sibling = parent->first_child;
	for (current_sibling = &dummy_head;
			NULL != current_sibling->next_sibling;
			current_sibling = current_sibling->next_sibling) {

		if (current_sibling->next_sibling == this) {
			current_sibling->next_sibling = this->first_child;
			this->last_child->next_sibling = this->next_sibling;
			break;
		}
	}

	/* Update prent's first_child and last_child pointers, if needed */
	if (parent->first_child == this)
		parent->first_child = this->first_child;
	if (parent->last_child == this)
		parent->last_child = this->last_child;

	/* Update parent's children count */
	parent->child_count += this->child_count - 1;

	return SUCCESS;
}

int remove_child(struct rnode *child)
{
	if (is_root(child)) return RM_CHILD_IS_ROOT;;

	struct rnode *parent = child->parent;
	struct rnode dummy_head;
	struct rnode *previous;
	int n;

	child->parent = NULL;

	/* Easy special case: parent has exactly one child. */
	if (1 == parent->child_count) {
		parent->first_child = NULL;
		parent->last_child = NULL;
		parent->child_count = 0;
		return 0;
	}

	/* Find node previous to child */
	dummy_head.next_sibling = parent->first_child;
	for (n = 0, previous = &dummy_head;
		NULL != previous->next_sibling;
		n++, previous = previous->next_sibling) 
		if (previous->next_sibling == child) 
			break;
	
	/* Update parent's first_child and last_child, if needed. */
	if (parent->first_child == child)
		parent->first_child = child->next_sibling;
	if (parent->last_child == previous->next_sibling)
			parent->last_child = previous;

	/* Skip 'child', effectively removing if from children list */
	previous->next_sibling = previous->next_sibling->next_sibling;

	parent->child_count --;
	return n;	
}

int insert_child(struct rnode *parent, struct rnode *insert, int index)
{
	struct rnode dummy_head, *current;
	int n;

	if (index < 0 || index > parent->child_count)
		return FAILURE;	/* invalid index */

	/* Find node just before insertion point */
	dummy_head.next_sibling = parent->first_child;
	for (	current = &dummy_head, n = index; 
		n > 0;
		current = current->next_sibling, n--);

	/* current is now the node before the insertion point */
	insert->next_sibling = current->next_sibling;
	current->next_sibling = insert;
	insert->parent = parent;

	/* Update parent's child pointers */
	if (0 == index)
		parent->first_child = insert;
	if (parent->child_count == index)
		parent->last_child = insert;

	return SUCCESS;
}

int swap_nodes(struct rnode *node)
{
	assert(NULL != node->parent);
	assert(is_root(node->parent));  /* must swap below root */

	struct rnode *parent = node->parent;
	char *length = strdup(node->edge_length_as_string);
	if(remove_child(node) < 0) return FAILURE;
	add_child(node, parent);

	free(node->edge_length_as_string);
	node->edge_length_as_string = strdup("");
	free(parent->edge_length_as_string);
	parent->edge_length_as_string = length;

	return SUCCESS;
}

int unlink_rnode(struct rnode *node)
{
	struct rnode *parent = node->parent;
	/* Remove this node from its parent's list of children.  */
	remove_child(node);

	/* If deleting this node results in the parent having only one child,
	 * we splice the parent out (unless it's the root, in which case we
	 * return its first child) */
	unlink_rnode_root_child = NULL;
	if (1 == parent->child_count) {
		if (is_root(parent)) {
			unlink_rnode_root_child = parent->first_child;
			return UNLINK_RNODE_ROOT_CHILD;
		}
		else {
			if (! splice_out_rnode(parent))
				return UNLINK_RNODE_ERROR;
			else
				return UNLINK_RNODE_DONE;
		}
	}
	return UNLINK_RNODE_DONE;
}

struct llist *siblings(struct rnode *node)
{
	struct rnode *sib, *parent;
	struct llist *result = create_llist();
	if (NULL == result) return NULL;

	if (is_root(node)) return result;

	parent = node->parent;
	for (sib = parent->first_child; NULL != sib; sib = sib->next_sibling)
		if (sib != node)
			if (! append_element(result, sib))
				return NULL;

	return result;
}

void remove_children(struct rnode *node)
{
	node->first_child = NULL;
	node->last_child = NULL;
	node->child_count = 0;
}
