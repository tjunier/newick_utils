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
#include <stdbool.h>

#include "tree.h"
#include "nodemap.h"
#include "parser.h"
#include "to_newick.h"
#include "rnode.h"
#include "link.h"
#include "hash.h"
#include "list.h"

struct parameters {
	struct llist 	*labels;
	bool		reverse;
};

void help(char *argv[])
{
	printf (
"Removes nodes by label\n"
"\n"
"Synopsis\n"
"--------\n"
"\n"
"%s [-hv] <newick trees filename|-> <label> [label+]\n"
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
"    -v: reverse: prune nodes whose labels are NOT passed on the command\n"
"        line. NOTE: this will also drop internal nodes if their labels\n"
"        are not passed. A future option will modify this.\n"
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
	params.reverse = false;

	int opt_char;
	while ((opt_char = getopt(argc, argv, "hv")) != -1) {
		switch (opt_char) {
		case 'h':
			help(argv);
			exit (EXIT_SUCCESS);
		case 'v':
			params.reverse = true;
			break;
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
			if (! append_element(lbl_list, argv[optind])) {
				perror(NULL);
				exit(EXIT_FAILURE);
			}
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
		/* parent may have been unlinked already, so let's check */
		if (NULL == goner->parent)
			continue;
		enum unlink_rnode_status result = unlink_rnode(goner);
		struct rnode *root_child;
		switch(result) {
		case UNLINK_RNODE_DONE:
			break;
		case UNLINK_RNODE_ROOT_CHILD:
			root_child = get_unlink_rnode_root_child();
			root_child->parent = NULL;
			tree->root = root_child;
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

/* Produces a new list with all labels in the tree that are NOT in 'labels'. */

struct llist *reverse_labels(struct rooted_tree *tree, struct llist *labels)
{
	struct llist *rev_labels = create_llist();
	/* We use a hash for looking up labels, instead of going over the list
	 * of labels every time (see below) */
	struct hash *labels_h = create_hash(labels->count);
	if (NULL == labels_h) {
		fprintf(stderr, "Memory error -exiting.\n");
		exit(EXIT_FAILURE);
	}

	struct list_elem *elem;

	/* fill label hash with the labels */
	char *PRESENT = "present";
	for (elem = labels->head; NULL != elem; elem = elem->next) {
		char *label = elem->data;
		if (! hash_set(labels_h, label, PRESENT)) {
			fprintf(stderr, "Memory error -exiting.\n");
			exit(EXIT_FAILURE);
		}
	}

	/* Now iterate over all nodes in the tree and see if their labels are
	 * found in the labels hash. If not, add them to the 'rev_labels' list.
	 * */
	for (elem=tree->nodes_in_order->head; NULL!=elem; elem=elem->next) {
		char *label = ((struct rnode*) elem->data)->label;
		if (0 == strcmp("", label)) continue;
		if (NULL == hash_get(labels_h, label)) {
			if (! append_element(rev_labels, label)) {
				fprintf(stderr, "Memory error -exiting.\n");
				exit(EXIT_FAILURE);
			}
		}
	}
	destroy_hash(labels_h);

	return rev_labels;
}

int main(int argc, char *argv[])
{
	struct rooted_tree *tree;	
	struct parameters params;
	
	params = get_params(argc, argv);

	while (NULL != (tree = parse_tree())) {
		if (params.reverse) {
			struct llist *rev_labels = reverse_labels(tree,
					params.labels);
			process_tree(tree, rev_labels);
			destroy_llist(rev_labels);
		} else {
			process_tree(tree, params.labels);
		}
		dump_newick(tree->root);
		/* NOTE: the tree was modified, but no nodes were added so 
		 * we can use destroy_tree() */
		destroy_all_rnodes(NULL);
		destroy_tree(tree);
	}

	destroy_llist(params.labels);

	return 0;
}
