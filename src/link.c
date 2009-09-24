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

// TODO: Check this!
#ifndef RNODE_DEF
#include "rnode.h"
#endif

#include "list.h"
#include "link.h"
#include "masprintf.h"
#include "common.h"

struct rnode *unlink_rnode_root_child;

int add_child(struct rnode *parent, struct rnode *child)
{
	struct llist *children_list;
	child->parent = parent;

	children_list = parent->children;
	if (! append_element(children_list, child))
		return FAILURE;

	return SUCCESS;
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
	/* link new node to this node */
	if (! add_child(new, this)) return FAILURE;
	free(this->edge_length_as_string);
	this->edge_length_as_string = strdup(new_edge_length);
	replace_child(parent, this, new);

	free(new_edge_length);

	return SUCCESS;
}
	
void replace_child (struct rnode *node, struct rnode *old, struct rnode *new)
{
	struct list_elem *el;

	for (el = node->children->head; NULL != el; el = el->next) { 
		if (el->data == old) {
			el->data = new;
			new->parent = node; /* only if old was found... */
			return;
		}
	}

	/* not found - do nothing */
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
	struct rnode *parent = this->parent;
	struct list_elem *elem;

	/* change the children's parent edges: they must now point to their
	 * 'grandparent', and the length from their parent to their grandparent
	 * must be added. */
	for (elem = this->children->head; NULL != elem; elem = elem->next) {
		struct rnode *child = elem->data;
		char *new_edge_len_s = add_len_strings(
			this->edge_length_as_string,
			child->edge_length_as_string);
		if (NULL == new_edge_len_s) return FAILURE;
		free(child->edge_length_as_string);
		child->edge_length_as_string = new_edge_len_s;
		child->parent = parent;  /* instead of this node */
	}
	struct llist *kids_copy = shallow_copy(this->children);
	if (NULL == kids_copy) return FAILURE;

	/* find where this node's parent edge is in parent */
	int i = llist_index_of(parent->children, this);

	/* delete old edge from parent's children list */
	struct llist *del = delete_after(parent->children, i-1, 1);
	if (NULL == del) return FAILURE;
	destroy_llist(del);

	/* insert list of modified edges in parent's children list */
	insert_after(parent->children, i-1, kids_copy);
	free(kids_copy);

	return SUCCESS;
}

int remove_child(struct rnode *child)
{
	if (is_root(child)) return RM_CHILD_IS_ROOT;;

	struct rnode *parent = child->parent;
	struct llist *kids = parent->children;
	int n = llist_index_of(kids, child);
	assert(n >= 0);
	struct llist *deleted = delete_after(kids, n-1, 1);
	if (NULL == deleted) return RM_CHILD_MEM_ERROR;
	child->parent = NULL;
	destroy_llist(deleted);

	return n;
}

// TODO: is this f() ever used?
int insert_child(struct rnode *parent, struct rnode *child, int index)
{
	struct llist *kids = parent->children;
	struct llist *insert = create_llist();
	if (NULL == insert) return FAILURE;
	if (! append_element(insert, child)) return FAILURE;
	insert_after(kids, index-1, insert);
	child->parent = parent;

	return SUCCESS;
}

// TODO: return value should differentiate between mem error and child-is-root
int swap_nodes(struct rnode *node)
{
	assert(NULL != node->parent);
	assert(is_root(node->parent));  /* must swap below root */

	struct rnode *parent = node->parent;
	char *length = strdup(node->edge_length_as_string);
	if(remove_child(node) < 0) return FAILURE;
	if (! add_child(node, parent)) return FAILURE;

	free(node->edge_length_as_string);
	node->edge_length_as_string = strdup("");
	free(parent->edge_length_as_string);
	parent->edge_length_as_string = length;

	return SUCCESS;
}

int unlink_rnode(struct rnode *node)
{
	struct rnode *parent = node->parent;
	struct llist *siblings = parent->children; 	/* includes 'node'! */
	int index = llist_index_of(siblings, node);
	/* This removes this node from its parent's list of children.  We get
	 * the resulting list only so we can free it. */
	struct llist *del = delete_after(siblings, index-1, 1);
	if (NULL == del) return UNLINK_RNODE_ERROR;
	destroy_llist(del);	

	/* If deleting this node results in the parent having only one child,
	 * we splice the parent out (unless it's the root, in which case we
	 * return its first child) */
	if (1 == siblings->count) {
		if (is_root(parent)) {
			unlink_rnode_root_child = 
				(struct rnode *) siblings->head->data;
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
	struct rnode *sib;
	struct llist *result = create_llist();
	if (NULL == result) return NULL;
	struct list_elem *elem;

	if (is_root(node)) return result;

	for (elem = node->parent->children->head; NULL != elem; elem = elem->next) {
		sib = (struct rnode *) elem->data;
		if (sib != node)
			if (! append_element(result, sib))
				return NULL;
	}

	return result;
}
