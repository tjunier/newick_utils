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
