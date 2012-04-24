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
#include <ctype.h>

#include "tree.h"
#include "nodemap.h"
#include "parser.h"
#include "to_newick.h"
#include "rnode.h"
#include "link.h"
#include "set.h"
#include "list.h"

/* define this to enable debugging of the decision process */
#define DEBUG_PRUNE_DECISION 1

enum prune_mode { PRUNE_DIRECT, PRUNE_REVERSE };
enum rev_inner_label_mode { PRUNE_IGNORE_ALL, PRUNE_IGNORE_NUMERIC,
	PRUNE_IGNORE_NONE };
enum prune_action { PRUNE_KEEP, PRUNE_PRUNE };

struct parameters {
	set_t 	*cl_labels;
	enum prune_mode mode;
	enum rev_inner_label_mode ilbl_mode;
};

void help(char *argv[])
{
	printf (
"Removes nodes by label\n"
"\n"
"Synopsis\n"
"--------\n"
"\n"
"%s [-hi:v] <newick trees filename|-> <label> [label+]\n"
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
"Only labeled nodes are considered for pruning.\n"
"\n"
"Options\n"
"-------\n"
"\n"
"    -h: print this message and exit\n"
"    -i <t|a>: changes the handling of inner nodes in reverse mode (see -v).\n"
"       If argument is 't' (text), inner nodes whose label is not passed\n" 
"       get pruned if the label is text (i.e., not numeric). If argument\n"
"       is 'a' (all), any internal node not specified on the command line\n"
"       is pruned, provided its label is not empty.\n"
"       This option allows the user to keep selected clades by specifying\n"
"       the name of their ancestor (see examples).\n"
"    -v: reverse: prune nodes whose labels are NOT passed on the command\n"
"        line. Inner nodes are not pruned, unless -i is also set (see\n"
"        above). This allows pruning of trees with support values, which\n"
"        syntactically are node labels, without inner nodes disappearing\n"
"        because their 'label' was not passed on the command line.\n"
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
"$ %s data/catarrhini Homininae\n"
"\n"
"# keep great apes and Colobines:\n"
"$ %s -v data/catarrhini Gorilla Pan Homo Pongo Simias Colobus\n"
"\n"
"# same, using clade labels:\n"
"$ %s -v -i t data/catarrhini Hominidae Colobinae\n"
"$ %s data/catarrhini Homininae\n",
	argv[0],
	argv[0],
	argv[0],
	argv[0],
	argv[0],
	argv[0]
	);
}

struct parameters get_params(int argc, char *argv[])
{
	struct parameters params;
	params.mode = PRUNE_DIRECT;
	params.ilbl_mode = PRUNE_IGNORE_ALL;

	int opt_char;
	while ((opt_char = getopt(argc, argv, "hi:v")) != -1) {
		switch (opt_char) {
		case 'h':
			help(argv);
			exit (EXIT_SUCCESS);
		case 'i':
			switch(tolower(optarg[0])) {
			case 't':
				params.ilbl_mode = PRUNE_IGNORE_NUMERIC;
				break;
			default:
				fprintf(stderr,
					"Invalid argument '%s' to -i.\n",
						optarg);
				exit(EXIT_FAILURE);
			}
			break;
		case 'v':
			params.mode = PRUNE_REVERSE;
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
		set_t *cl_labels = create_set();
		if (NULL == cl_labels) { perror(NULL); exit(EXIT_FAILURE); }
		optind++;	/* optind is now index of 1st label */
		for (; optind < argc; optind++) {
			if (set_add(cl_labels, argv[optind]) < 0) {
				perror(NULL);
				exit(EXIT_FAILURE);
			}
		}
		params.cl_labels = cl_labels;
	} else {
		fprintf(stderr, "Usage: %s [-h] <filename|-> <label> [label+]\n",
				argv[0]);
		exit(EXIT_FAILURE);
	}

	return params;
}

/* Return true IFF string is numeric (incl. float), see http://rosettacode.org/wiki/Determine_if_a_string_is_numeric#C */

static bool is_numeric (const char * s)
{
	if (NULL == s || '\0' ==  *s  || isspace(*s))
		return 0;
	char * p;
	strtod (s, &p);
	return *p == '\0';
}

static enum prune_action decide_action(struct rnode *node, set_t cl_labels,
		enum prune_mode mode))
{
		
	/* true IFF label is passed in the command line */
	bool current_in_cl = set_has_element(cl_labels,
			node->label);

	/* Keep nodes with empty labels */
	if (0 == strcmp(node->label, "")) {
		return PRUNE_KEEP;
	}

	/* In direct mode, keep nodes NOT passed as arg... */ 
	if (PRUNE_DIRECT == mode && !current_in_cl) {
		return PRUNE_KEEP;
	}
	/* ...but in reverse mode... */
	if (PRUNE_REVERSE == mode) {
		if (current_in_cl) {
			/* keep nodes that ARE passed... */
			return PRUNE_KEEP;
		}
		else if (is_inner_node(current_node)) {
			/* unless they're inner nodes... */
			switch (ilbl_mode) {
			case PRUNE_IGNORE_ALL: continue;
			case PRUNE_IGNORE_NUMERIC:
				if (is_numeric(current_node->label))
					continue;
				else
					break;
			case PRUNE_IGNORE_NONE: break;
			default:
				assert(0);
			}
		}
	}

		
	/* any node that "falls through" gets pruned  */
	return PRUNE_PRUNE;
}

/* We build a hash of the passed labels, and go through the tree in reverse
 * Newick order, unlinking nodes as needed (and [eventually] preventing further
 * visiting, as in nw_ed). This will entail at most one passage through the
 * tree, ensure that descendants of removed nodes are not processed, and allow
 * a single hash to be constructed for all the trees. In fact, if the labels
 * list is short, one does not need a hash at all.  */

static void process_tree(struct rooted_tree *tree, set_t *cl_labels,
		enum prune_mode mode, enum rev_inner_label_mode ilbl_mode)
{
	struct list_elem *elem;

	for (elem = tree->nodes_in_order->head; NULL != elem;
			elem = elem->next) {
		struct rnode *current_node = elem->data;

		enum prune_action action = prune_action(current_rnode);

		enum unlink_rnode_status result = unlink_rnode(current_node);

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
		case UNLINK_RNODE_ROOT:
			fprintf(stderr, "WARNING: root cannot be pruned.\n");
			break;
		default:
			assert(0); /* programmer error */
		}
	}
}

int main(int argc, char *argv[])
{
	struct rooted_tree *tree;	
	struct parameters params;
	
	params = get_params(argc, argv);

	while (NULL != (tree = parse_tree())) {
		process_tree(tree, params.cl_labels, params.mode, 
				params.ilbl_mode);
		dump_newick(tree->root);
		/* NOTE: the tree was modified, but no nodes were added so 
		 * we can use destroy_tree() */
		destroy_all_rnodes(NULL);
		destroy_tree(tree);
	}

	destroy_set(params.cl_labels);

	return 0;
}
