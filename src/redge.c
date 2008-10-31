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

	// TODO: just use strdup(), which should take care of allocating
	// memory. 
	l = strlen(length_s) + 1;
	redgep->length_as_string = malloc(l * sizeof(char));
	if (NULL == redgep->length_as_string) {
		perror(NULL);
		exit(EXIT_FAILURE);
	}
        /* NOTE: length_as_string is never NULL (at least it points to ""), so
         * we can use this in assert() statements */

#ifdef SHOW_REDGE_CREATE
	fprintf (stderr, "creating redge %p (length %s at %p)\n",  redgep, length_s, redgep->length_as_string);
#endif
	strncpy(redgep->length_as_string, length_s, l);

	return redgep;
}

void destroy_redge(struct redge *edge)
{
#ifdef SHOW_REDGE_DESTROY
	fprintf(stderr, " freeing redge %p (length %s at %p)\n",
		edge, edge->length_as_string, edge->length_as_string);
#endif
	free(edge->length_as_string);
	free(edge);
}

void dump_redge(void *arg) {
	struct redge *edge = (struct redge *) arg;
	printf ("edge ");
	if (NULL != edge->length_as_string)
		printf(" [%s] ", edge->length_as_string);
	printf ("to %s\n", edge->child_node->label);
}
