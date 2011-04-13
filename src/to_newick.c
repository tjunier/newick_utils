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
#include <stdbool.h>

#include "rnode.h"
#include "list.h"
#include "concat.h"
#include "common.h"
#include "rnode_iterator.h"
#include "hash.h"
#include "masprintf.h"

static bool show_addresses = false;

/* When show_addresses is true, the node's addresses appear in the Newick
 * string, which can be quite handy for debugging. */

void set_show_addresses(bool show) { show_addresses = show; }

/* returns the length part of a node, e.g. ":12.345" */

char *length(struct rnode *node)
{
	char * result = malloc(sizeof(char));
	if (NULL == result) return NULL;
	*result = '\0';

	assert(NULL != node->edge_length_as_string);

	if (strlen(node->edge_length_as_string) > 0) {
		result = append_to(result, ":");
		result = append_to(result,
			       node->edge_length_as_string);	
	}

	return result;
}

char *subtree(struct rnode *node)
{
	char * result = malloc(sizeof(char));
	if (NULL == result) return NULL;
	*result = '\0';

	if (is_leaf(node)) {
		result = append_to(result, node->label);
		char *length_s = length(node);
		if (NULL == length_s) return NULL;
		result = append_to(result, length_s);
		free(length_s);
	} else {
		struct rnode *child;
		char * child_node_s;

		result = append_to(result, "(");

		/* first child */
		child = node->first_child;
		child_node_s = subtree(child);
		if (NULL == child_node_s) return NULL;
		result = append_to(result, child_node_s);
		free(child_node_s);
		/* other children, comma-separated */
		for (child = child->next_sibling;
				NULL != child; child = child->next_sibling) {
			result = append_to(result, ",");
			child_node_s = subtree(child);
			if (NULL == child_node_s) return NULL;
			result = append_to(result, child_node_s);
			free(child_node_s);
		}
		result = append_to(result, ")");
		if (NULL != node->label) {
			result = append_to(result, node->label);
		}
		char *length_s = length(node);
		if (NULL == length_s) return NULL;
		result = append_to(result, length_s);
		free(length_s);
	}
	return result;
}

char *to_newick(struct rnode *node)
{
	char *result;
	result = subtree(node);
	if (NULL == result) return NULL;
	result = append_to(result, ";");
	return result;
}

/* A helper function for to_newick_i(). Appends to 'result' strings
 * representing a leaf. */
/* Return value indicates SUCCESS or FAILURE. */

static int append_leaf(struct llist *result, struct rnode *current)
{
	append_element(result, strdup(current->label));
	if (show_addresses) 
		if (! append_element(result, masprintf("@%p", current)))
			return FAILURE;
	if (strcmp("", current->edge_length_as_string) != 0) {
		if (! append_element(result, strdup(":")))
			return FAILURE;
		if (! append_element(result,
			strdup(current->edge_length_as_string)))
			return FAILURE;
	}

	return SUCCESS;
}

/* A helper function for to_newick_i(). Appends to 'result' strings
 * representing the end of an inner node. This always contains a ')', and may
 * also contain a label and a length (plus the node's address, if
 * show_addresses is true.) */
/* Also, takes care of resetting the 'seen' member of the node. */
/* Return value indicates SUCCESS or FAILURE. */

static int append_inner_node_end(struct llist *result,
		struct rnode *current)
{
	if (! append_element(result, strdup(")")))
		return FAILURE;
	if (strcmp("", current->label) != 0)
		if (! append_element(result, strdup(current->label)))
			return FAILURE;
	if (show_addresses) 
		if (! append_element( result, masprintf("@%p", current)))
			return FAILURE;
	if (strcmp("", current->edge_length_as_string) != 0) {
		if (! append_element(result, strdup(":")))
			return FAILURE;
		if (! append_element(result,
				strdup(current->edge_length_as_string)))
			return FAILURE;
	}
	current->seen = 0;	/* reset */

	return SUCCESS;
}

/* A helper function for to_newick_i(). Appends to 'result' strings
 * representing an inner node. */
/* Return value indicates SUCCESS or FAILURE. */

static int append_inner_node(struct rnode_iterator *it,
		struct llist *result, struct rnode *current)
{
	/* inner node: behaviour depends on whether we've
	 * already 'seen' this node or not. */
	if (0 == current->seen) {
		/* not seen: print '(' */
		current->seen = 1;
		if (! append_element(result, strdup("(")))
			return FAILURE;
	} else {
		if (more_children_to_visit(it)) {
			if (! append_element(result, strdup(",")))
				return FAILURE;
		}
		else 
			/* append ')' and possibly label and lenth */
			if (! append_inner_node_end(result, current))
				return FAILURE;
	}

	return SUCCESS;
}

struct llist *to_newick_i(struct rnode *node)
{
	struct rnode_iterator *it;
	struct rnode *current;
	struct llist *result = create_llist();

	it = create_rnode_iterator(node);
	if (NULL == it) return NULL;
	
	struct rnode *it_root = get_rnode_iterator_root(it);
	it_root->seen = 1;

	if (! is_leaf(it_root))
		if (! append_element(result, strdup("(")))
			return NULL;

	while ((current = rnode_iterator_next(it)) != NULL) {
		if (is_leaf(current)) {
			if (! append_leaf(result, current))
				return NULL;
		}
		else {
			if (! append_inner_node(it, result, current))
				return NULL;
		}
	}
	if (! append_element(result, strdup(";")))
		return NULL;

	destroy_rnode_iterator(it);

	return result;
}

int dump_newick(struct rnode *node)
{
	struct llist *nw_strings = to_newick_i(node);
	if (NULL == nw_strings) return FAILURE;

	struct list_elem *e;

	for (e = nw_strings->head; NULL != e; e = e->next) 
		printf("%s", (char *) e->data);
	printf("\n");

	for (e = nw_strings->head; NULL != e; e = e->next) 
		free(e->data);
	destroy_llist(nw_strings);

	return SUCCESS;
}
