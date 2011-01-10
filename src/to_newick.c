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
#include "concat.h"
#include "common.h"
#include "rnode_iterator.h"
#include "hash.h"

// TODO: make all functions static unless needed otherwise

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

struct llist *to_newick_i(struct rnode *node)
{
	struct rnode_iterator *it;
	struct rnode *current;
	struct llist *result = create_llist();

	it = create_rnode_iterator(node);
	if (NULL == it) {
		// TODO: set error msg, or sthg
		return NULL;
	}
	
	it->root->seen = 1;

	if (! is_leaf(it->root))
		append_element(result, strdup("("));

	while ((current = rnode_iterator_next(it)) != NULL) {
		if (is_leaf(current)) {
			/* leaf: just print label */
			append_element(result, strdup(current->label));
			if (strcmp("", current->edge_length_as_string) != 0) {
				append_element(result, strdup(":"));
				append_element(result,
					strdup(current->edge_length_as_string));
			}
		}
		else {
			/* inner node: behaviour depends on whether we've
			 * already 'seen' this node or not. */
			if (0 == current->seen) {
				/* not seen: print '(' */
				current->seen = 1;
				append_element(result, strdup("("));
			} else {
				if (more_children_to_visit(it)) {
					append_element(result, strdup(","));
				}
				else {
					//printf(")%s", current->label);
					append_element(result, strdup(")"));
					if (strcmp("", current->label) != 0)
						append_element(result,
							strdup(current->label));
					if (strcmp("",
						current->edge_length_as_string) != 0) {
						//printf(":%s", current->edge_length_as_string);
						append_element(result,
								strdup(":"));
						append_element(result,
								strdup(current->edge_length_as_string));
					}
					current->seen = 0;	/* reset */
				}
			}
		}
	}
	//printf(";\n");
	append_element(result, strdup(";"));

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

	return SUCCESS;
}
