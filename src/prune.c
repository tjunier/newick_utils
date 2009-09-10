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
/* prune.c: remove nodes from tree */

#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "tree.h"
#include "nodemap.h"
#include "parser.h"
#include "to_newick.h"
#include "rnode.h"
#include "link.h"
#include "hash.h"
#include "list.h"

struct parameters {
	struct llist *labels;
};

void help(char *argv[])
{
	printf (
"Removes nodes by label\n"
"\n"
"Synopsis\n"
"--------\n"
"\n"
"%s [-h] <newick trees filename|->\n"
"\n"
"Input\n"
"-----\n"
"\n"
"Argument is the name of a file that contains Newick trees, or '-' (in which\n"
"case trees are read from standard input).\n"
"\n"
"Output\n"
"------\n"
"\n"
"Removes all nodes whose labels are passed on the command line, and prints\n"
"out the modified tree. If removing a node causes its parent to have only\n"
"one child (as is always the case in strictly binary trees), the parent is\n"
"spliced out and the remaining child is attached to its grandparent,\n"
"preserving length.\n"
"\n"
"Options\n"
"-------\n"
"\n"
"    -h: print this message and exit\n"
"\n"
"Assumptions and Limitations\n"
"---------------------------\n"
"\n"
"Labels are assumed to be unique. \n"
"\n"
"Examples\n"
"--------\n"
"\n"
"# Remove humans and gorilla\n"
"$ %s data/catarrhini Homo Gorilla\n"
"\n"
"# Remove humans, chimp, and gorilla\n"
"$ %s data/catarrhini Homo Gorilla Pan\n"
"\n"
"# the same, but using the clade's label\n"
"$ %s data/catarrhini Homininae\n",
	argv[0],
	argv[0],
	argv[0],
	argv[0]
	);
}

struct parameters get_params(int argc, char *argv[])
{
	struct parameters params;

	int opt_char;
	while ((opt_char = getopt(argc, argv, "h")) != -1) {
		switch (opt_char) {
		case 'h':
			help(argv);
			exit (EXIT_SUCCESS);
		default:
			fprintf (stderr, "Unknown option '-%c'\n", opt_char);
			exit (EXIT_FAILURE);
		}
	}

	/* check arguments */
	if ((argc - optind) >= 2)	{
		if (0 != strcmp("-", argv[optind])) {
			FILE *fin = fopen(argv[optind], "r");
			extern FILE *nwsin;
			if (NULL == fin) {
				perror(NULL);
				exit(EXIT_FAILURE);
			}
			nwsin = fin;
		}
		struct llist *lbl_list = create_llist();
		if (NULL == lbl_list) { perror(NULL); exit(EXIT_FAILURE); }
		optind++;	/* optind is now index of 1st label */
		for (; optind < argc; optind++) {
			append_element(lbl_list, argv[optind]);
		}
		params.labels = lbl_list;
	} else {
		fprintf(stderr, "Usage: %s [-h] <filename|-> <label> [label+]\n",
				argv[0]);
		exit(EXIT_FAILURE);
	}

	return params;
}

void process_tree(struct rooted_tree *tree, struct llist *labels)
{
	struct hash *lbl2node_map = create_label2node_map(tree->nodes_in_order);
	struct list_elem *elem;

	for (elem = labels->head; NULL != elem; elem = elem->next) {
		char *label = elem->data;
		struct rnode *goner = hash_get(lbl2node_map, label);
		if (NULL == goner) {
			fprintf (stderr, "WARNING: label '%s' not found.\n",
					label);
			continue;
		}
		enum unlink_rnode_status result = unlink_rnode(goner);
		switch(result) {
		case UNLINK_RNODE_DONE:
			break;
		case UNLINK_RNODE_ROOT_CHILD:
			unlink_rnode_root_child->parent = NULL;
			tree->root = unlink_rnode_root_child;
			break;
		case UNLINK_RNODE_ERROR:
			fprintf (stderr, "Memory error - exiting.\n");
			exit(EXIT_FAILURE);
		default:
			assert(0); /* programmer error */
		}
	}

	destroy_hash(lbl2node_map);
}

int main(int argc, char *argv[])
{
	struct rooted_tree *tree;	
	struct parameters params;
	
	params = get_params(argc, argv);

	while (NULL != (tree = parse_tree())) {
		process_tree(tree, params.labels);
		char *newick = to_newick(tree->root);
		printf ("%s\n", newick);
		free(newick);
		/* NOTE: the tree was modified, but no nodes were added so it
		 * should be possible to use destroy_tree() */
		destroy_tree(tree, DONT_FREE_NODE_DATA);
	}

	destroy_llist(params.labels);

	return 0;
}
