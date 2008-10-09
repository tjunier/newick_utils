#include <stdlib.h>
#include <string.h>

#include "tree.h"
#include "list.h"
#include "rnode.h"

void underscores2spaces(char *s)
{
	char *p;
	for (p = s; '\0' != *p; p++)
		if ('_' == *p)
			*p = ' ';
}

void remove_quotes(char *s)
{
	char *tmp = strdup(s);
	char *c;
	int i = 0;

	for (c = tmp; '\0' != *c; c++)
		switch (*c) {
		// maybe a case for double quotes here later?
		case '\'':
			break;
		default:
			s[i] = *c;
			i++;
		}
	s[i] = '\0';

	free(tmp);
}

void prettify_labels (struct rooted_tree *tree)
{
	struct list_elem *el;
	for (el = tree->nodes_in_order->head; NULL != el; el = el->next) {
		struct rnode *current = el->data;
		underscores2spaces(current->label);
		remove_quotes(current->label);
	}
}
