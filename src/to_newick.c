#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "rnode.h"
#include "redge.h"
#include "list.h"
#include "concat.h"

/* returns the length part of a node, e.g. ":12.345" */

char *length(struct rnode *node)
{
	char * result = malloc(sizeof(char));
	if (NULL == result) {
		perror(NULL);
		exit(EXIT_FAILURE);
	}
	*result = '\0';

	if (NULL != node->parent_edge) {
		if (NULL != node->parent_edge->length_as_string) {
			if (strlen(node->parent_edge->length_as_string) > 0) {
				result = append_to(result, ":");
				result = append_to(result,
					       node->parent_edge->length_as_string);	
			}
		}
	}

	return result;
}

char *subtree(struct rnode *node)
{
	char * result = malloc(sizeof(char));
	if (NULL == result) {
		perror(NULL);
		exit(EXIT_FAILURE);
	}
	*result = '\0';

	if (is_leaf(node)) {
		result = append_to(result, node->label);
		char *length_s = length(node);
		result = append_to(result, length_s);
		free(length_s);
	} else {
		struct list_elem *elem;
		struct redge *edge;
		char * child_node_s;

		result = append_to(result, "(");

		/* first child */
		elem = node->children->head;
		edge = (struct redge *) elem->data;
		child_node_s = subtree(edge->child_node);
		result = append_to(result, child_node_s);
		free(child_node_s);
		/* other children, comma-separated */
		for (elem=elem->next; elem!=NULL; elem=elem->next) {
			result = append_to(result, ",");
			edge = (struct redge *) elem->data;
			child_node_s = subtree(edge->child_node);
			result = append_to(result, child_node_s);
			free(child_node_s);
		}
		result = append_to(result, ")");
		if (NULL != node->label) {
			result = append_to(result, node->label);
		}
		char *length_s = length(node);
		result = append_to(result, length_s);
		free(length_s);
	}
	return result;
}

char *to_newick(struct rnode *node)
{
	char *result;
	result = subtree(node);
	result = append_to(result, ";");
	return result;
}
