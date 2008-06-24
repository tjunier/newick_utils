#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "redge.h"
#include "rnode.h"

struct redge *create_redge(char *length_s)
{
	struct redge *redgep;
	int l;
	redgep = malloc(sizeof(struct redge));
	if (NULL == redgep) {
		perror(NULL);
		exit(EXIT_FAILURE);
	}

	redgep->parent_node = NULL;
	redgep->child_node = NULL;

	/* A NULL length string means that the Newick does not specify length,
	 * which is different from a length of zero or a negative length (which
	 * can happen with NJ trees) */

	if (NULL == length_s) {
		length_s = "";
	}

	l = strlen(length_s) + 1;
	redgep->length_as_string = malloc(l * sizeof(char));
	if (NULL == redgep->length_as_string) {
		perror(NULL);
		exit(EXIT_FAILURE);
	}
	strncpy(redgep->length_as_string, length_s, l);

	return redgep;
}

void dump_redge(void *arg) {
	struct redge *edge = (struct redge *) arg;
	printf ("edge ");
	if (NULL != edge->length_as_string)
		printf(" [%s] ", edge->length_as_string);
	printf ("to %s\n", edge->child_node->label);
}
