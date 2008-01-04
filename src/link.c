#define _GNU_SOURCE

#include <stdlib.h>
#include <string.h>
#include <stdio.h>


#ifndef RNODE_DEF
#include "rnode.h"
#endif

#include "redge.h"
#include "list.h"
#include "link.h"

void add_child_edge(struct rnode *parent, struct redge *edge)
{
	struct llist *children_list;
	edge->parent_node = parent;

	children_list = parent->children;
	append_element(children_list, edge);
}

void set_parent_edge(struct rnode *child, struct redge *edge)
{
	child->parent_edge = edge;
	edge->child_node = child;
}

void link_p2c(struct rnode *parent, struct rnode *child, char *length_s)
{
	struct redge *edge;

	edge = create_redge(length_s);

	edge->parent_node = parent;

	add_child_edge(parent, edge);
	set_parent_edge(child, edge);
}

/* Returns half the length passed as a parameter (as char *), or "". */

char * compute_new_edge_length(char * length_as_string)
{
	char *result = "";

	if (0 != strcmp("", length_as_string)) {
		double length = atof(length_as_string);
		/* TODO: asprintf() is a GNU extension - later replace it with
		 * an in-house version that does not require nonstandard
		 * extensions. */
		asprintf(&result, "%g", length / 2);
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
	parent_edge = this->parent_edge;
	parent = parent_edge->parent_node;
	new_edge_length = compute_new_edge_length(parent_edge->length_as_string);

	/* create new node */
	new = create_rnode(label);
	/* link new node to this node */
	link_p2c(new, this, new_edge_length);
	/* for the new node's parent edge, we can't just call link_p2c(),
	   because we need to replace the old child edge with the new. */
	new_node_parent_edge = create_redge(new_edge_length);
	set_parent_edge(new, new_node_parent_edge);
	replace_child_edge(parent, parent_edge, new_node_parent_edge);
	new_node_parent_edge->parent_node = parent;

	/* destroy parent edge */
	free(parent_edge);
}
	
void replace_child_edge(struct rnode *node, struct redge *old, struct redge *new)
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
	char * result = "";

	/* if ls1 and ls2 are not both "" */
	if (	strcmp("", ls1) != 0 ||
		strcmp("", ls2) != 0)	{
		double l1 = atoi(ls1);
		double l2 = atoi(ls2);
		asprintf(&result, "%g", l1 + l2);
	}

	return result;
}

void splice_out_rnode(struct rnode *this)
{
	/* get parent edge and its length */
	struct redge *parent_edge = this->parent_edge;
	struct rnode *parent = parent_edge->parent_node;
	struct list_elem *elem;

	/* replace each child's parent edge with an edge to the child's grandparent */
	/* collect new edges into a list */

	struct llist *new_edges_list = create_llist();
	for (elem = this->children->head; NULL != elem; elem = elem->next) {
		struct redge *child_edge = (struct redge *) elem->data;
		struct rnode *child = child_edge->child_node;
		char *new_edge_len_s = add_len_strings(parent_edge->length_as_string,
				child_edge->length_as_string);
		struct redge *new_edge = create_redge(new_edge_len_s);
		new_edge->parent_node = parent;	/* instead of this node */
		set_parent_edge(child, new_edge);
		append_element(new_edges_list, new_edge);
	}

	/* find where this node's parent edge is in parent */
	int i = 0;
	for (elem = parent->children->head; NULL != elem; elem = elem->next) {
		struct redge *sibling_edge = (struct redge *) elem->data;
		if (sibling_edge == parent_edge) 
			break;
		else
			i++;
	}

	/* delete old edge from parent's children list */
	delete_after(parent->children, i-1, 1);

	/* insert list of new edges in parent's children list */
	insert_after(parent->children, i-1, new_edges_list);

}

void reverse_redge(struct redge *edge)
{
	struct rnode *parent, *child;
	struct redge *reverse_edge;

	parent = edge->parent_node;
	child = edge->child_node;

	/* remove edge from old parent's children list */
	int n = llist_index_of(parent->children, edge);
	delete_after(parent->children, n - 1, 1);

	/* create new edge with same length string as old */
	reverse_edge = create_redge(edge->length_as_string);

	set_parent_edge(parent, reverse_edge);	/* intentional (reversing!) */

	add_child_edge(child, reverse_edge); 	/* intentional (reversing!) */
	reverse_edge->parent_node = child;
	child->parent_edge = NULL;	
}
