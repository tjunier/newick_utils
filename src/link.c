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

// TODO: Check this!
#ifndef RNODE_DEF
#include "rnode.h"
#endif

#include "list.h"
#include "link.h"
#include "masprintf.h"

void add_child(struct rnode *parent, struct rnode *child)
{
	struct llist *children_list;
	child->parent = parent;

	children_list = parent->children;
	append_element(children_list, child);
}

/* OBSOLETE - use add_child() */
/*
void set_parent_edge(struct rnode *child, struct redge *edge)
{
	child->parent_edge = edge;
	edge->child_node = child;
}
*/

/* OBSOLETE - use add_child() */
/*
void link_p2c(struct rnode *parent, struct rnode *child, char *length_s)
{
	struct redge *edge;

	edge = create_redge(length_s);

	edge->parent_node = parent;

	add_child_edge(parent, edge);
	set_parent_edge(child, edge);
}
*/

/* Returns half the length passed as a parameter (as char *), or "". */

char * compute_new_edge_length(char * length_as_string)
{
	char *result;

	if (0 != strcmp("", length_as_string)) {
		double length = atof(length_as_string);
		result = masprintf("%g", length / 2);
		if (NULL == result) { perror(NULL); exit(EXIT_FAILURE); }
	} else  {
                /* Note: we cannot just say result = ""; because result will be
                 * free()d later, so it has to have been dynamically allocated.
                 * */
                result = strdup("");
        }

	return result;
}

void insert_node_above(struct rnode *this, char *label)
{
	struct redge *parent_edge;
	struct rnode *parent;
	struct rnode *new;
	struct redge *new_node_parent_edge;
	char * new_edge_length;	/* both new edges have 1/2 the length of the old one */

	/* find parent edge and parent node */
	parent = this->parent;
	new_edge_length = compute_new_edge_length(parent->edge_length_as_string);
	/* create new node */
	new = create_rnode(label, new_edge_length);
	/* link new node to this node */
	// link_p2c(new, this, new_edge_length);
	/* for the new node's parent edge, we can't just call link_p2c(),
	   because we need to replace the old child edge with the new. */
	// new_node_parent_edge = create_redge(new_edge_length);
	// set_parent_edge(new, new_node_parent_edge);
	replace_child(parent, this, new);

	free(new_edge_length);
}
	
void replace_child (struct rnode *node, struct rnode *old, struct rnode *new)
{
	struct list_elem *el;

	for (el = node->children->head; NULL != el; el = el->next) { 
		if (el->data == old) {
			el->data = new;
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
		if (NULL == result) { perror(NULL); exit(EXIT_FAILURE); }
	} else {
		result = strdup("");
		/* I'd be REALLY suprised if this fails... */
		if (NULL == result) { perror(NULL); exit(EXIT_FAILURE); }
	}

	return result;
}

/* 'this' node is the one that is to be spliced out. All nodes and edges are
 * relative to this one. */

void splice_out_rnode(struct rnode *this)
{
	/* get this node parent edge and its length */
	struct rnode *parent = this->parent;
	struct list_elem *elem;

	/* change the children's parent edges: they must now point to their
	 * 'grandparent', and the length from their parent to their grandparent
	 * must be added. */
	for (elem = this->children->head; NULL != elem; elem = elem->next) {
		struct rnode *child = (struct rnode *) elem->data;
		char *new_edge_len_s = add_len_strings(
			parent->edge_length_as_string,
			child->edge_length_as_string);
		free(child->edge_length_as_string);
		child->edge_length_as_string = new_edge_len_s;
		child->parent = parent;  /* instead of this node */
	}
	struct llist *kids_copy = shallow_copy(this->children);

	/* find where this node's parent edge is in parent */
	int i = llist_index_of(parent->children, parent);

	/* delete old edge from parent's children list */
	struct llist *del = delete_after(parent->children, i-1, 1);
	destroy_llist(del);

	/* insert list of modified edges in parent's children list */
	insert_after(parent->children, i-1, kids_copy);
	free(kids_copy);
}

/* OBSOLETE */
/*
void reverse_redge(struct redge *edge)
{
	struct rnode *parent, *child;
	struct redge *reverse_edge;

	parent = edge->parent_node;
	child = edge->child_node;

	// remove edge from old parent's children list 
	int n = llist_index_of(parent->children, edge);
	struct llist *deleted = delete_after(parent->children, n - 1, 1);
        destroy_llist(deleted);

	// create new edge with same length string as old
	reverse_edge = create_redge(edge->length_as_string);

	set_parent_edge(parent, reverse_edge);	// intentional (reversing!)

	add_child_edge(child, reverse_edge); 	// intentional (reversing!)
	reverse_edge->parent_node = child;
	child->parent_edge = NULL;	
	destroy_redge(edge);
}
*/

struct rnode * unlink_rnode(struct rnode *node)
{
	struct rnode *parent = node->parent;
	struct llist *siblings = parent->children; 	/* includes 'node'! */
	int index = llist_index_of(siblings, parent);
	/* This removes this node from its parent's list of children.  We get
	 * the resulting list only so we can free it. */
	struct llist *del = delete_after(siblings, index-1, 1);
	destroy_llist(del);	

	/* If deleting this node results in the parent having only one child,
	 * we splice the parent out (unless it's the root, in which case we
	 * return its first child) */
	if (1 == siblings->count) {
		if (is_root(parent)) {
			return (struct rnode *) siblings->head->data;
		}
		else {
			splice_out_rnode(parent);
			return NULL;
		}
	}
	return NULL;
}

struct llist *siblings(struct rnode *node)
{
	struct rnode *sib;
	struct llist *result = create_llist();
	struct list_elem *elem;

	if (is_root(node)) return result;

	for (elem = node->parent->children->head; NULL != elem; elem = elem->next) {
		sib = (struct rnode *) elem->data;
		if (sib != node)
			append_element(result, sib);
	}

	return result;
}
