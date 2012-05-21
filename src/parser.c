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
#include <stdio.h>
#include <stdlib.h>

#include "list.h"
// #include "newick.tab.h"
#include "tree.h"
#include "parser.h"
#include "common.h"

struct llist *nodes_in_order;
struct rnode *root;
enum parser_status_type newick_parser_status;

int nwsparse(); 

int set_parser_input_filename (char *filename)
{
	extern FILE *nwsin;
	FILE *fin = fopen(filename, "r");
	if (NULL == fin) return FAILURE;
	nwsin = fin;

	return SUCCESS;
}

struct rooted_tree *parse_tree()
{
	struct rooted_tree *tree;

	tree = malloc(sizeof(struct rooted_tree));
	if(NULL == tree) {
		newick_parser_status = PARSER_STATUS_MALLOC_ERROR; 
		return NULL;
	}

	nodes_in_order = create_llist();
	if (NULL == nodes_in_order) {
		newick_parser_status = PARSER_STATUS_MALLOC_ERROR;
		return NULL;
	}

	/* calls the YACC (Bison, in fact) parser. This sets 'root' and
	 * 'newick_parser_status'. */
	nwsparse();
	
	if (NULL != root) {
		tree->root = root;
		tree->nodes_in_order = nodes_in_order;
		tree->type = TREE_TYPE_UNKNOWN; 
		return tree;
	} else {
		free(tree);
		destroy_llist(nodes_in_order);
		/* NOTE: 'newick_parser_status' has been set by nwsparse(), and
		 * can be read by caller (should, in fact). */
		return NULL;
	}
}
