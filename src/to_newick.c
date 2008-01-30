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
	char *result = "";

	if (NULL != node->parent_edge) {
		if (NULL != node->parent_edge->length_as_string) {
			if (strlen(node->parent_edge->length_as_string) > 0) {
				result = concat(result, ":");
				result = concat(result,
					       node->parent_edge->length_as_string);	
			}
		}
	}

	return result;
}

char *indent_string(char *tab, const int n)
{
	char *result = "";
	int i;

	for (i = 0; i < n; i++) 
		result = concat(result, tab);
	
	return result;
}

char *subtree(struct rnode *node)
{
	char * result;

	if (is_leaf(node)) {
		result = concat("", node->label);
		result = concat(result, length(node));
	} else {
		struct list_elem *elem;
		struct redge *edge;

		//result = concat(result, "(");
		result = "(";

		/* first child */
		elem = node->children->head;
		edge = (struct redge *) elem->data;
		result = concat(result, subtree(edge->child_node));
		/* other children, comma-separated */
		for (elem=elem->next; elem!=NULL; elem=elem->next) {
			result = concat(result, ",");
			edge = (struct redge *) elem->data;
			result = concat(result, subtree(edge->child_node));
		}
		result = concat(result, ")");
		if (NULL != node->label) { result = concat(result, node->label); }
		result = concat(result, length(node));
	}
	return result;
}

char *to_newick(struct rnode *node)
{
	char *result;
	result = subtree(node);
	result = concat(result, ";");
	return result;
}
