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
#include "parser.h"
#include "to_newick.h"
#include "rnode.h"
#include "link.h"
#include "set.h"
#include "list.h"

enum prune_mode { PRUNE_DIRECT, PRUNE_REVERSE };

struct parameters {
	set_t 	*cl_labels;
	enum prune_mode mode;
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

	int opt_char;
	while ((opt_char = getopt(argc, argv, "hi:v")) != -1) {
		switch (opt_char) {
		case 'h':
			help(argv);
			exit (EXIT_SUCCESS);
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

/* We buld a hash of the passed labels, and visit the tree, unlinking nodes as
 * needed. In Direct mode, we traverse in reverse order, unlinking nodes to
 * prune. In Reverse mode, things are a bit more hairy because we need to keep
 * not just the nodes passed as argument, but also their ancestors. So we pass
 * through th etree first in direct order, marking nodes to keep and their
 * ancestors. Then we go back in reverse order, pruning. */

static struct rooted_tree * process_tree_direct(
		struct rooted_tree *tree, set_t *cl_labels)
{
	struct llist *rev_nodes = llist_reverse(tree->nodes_in_order);
	struct list_elem *el;
	struct rnode *current;
	char *label;

	for (el = rev_nodes->head; NULL != el; el = el->next) {
		current = el->data;
		label = current->label;
		/* skip this node iff parent is marked ("seen") */
		if (!is_root(current) && current->parent->seen) {
			current->seen = true;	/* inherit mark */
			fprintf(stderr, "skipped: %s\n", label);
			continue;
		}
		if (set_has_element(cl_labels, label)) {
			unlink_rnode(current);
			current->seen = true;
			fprintf(stderr, "goner: %s\n", label);
		}
	}

	destroy_llist(rev_nodes);
	reset_seen(tree);
	return tree;
}

/* Prune predicate: returns true iff child node must be _kept_. In our case,
 * this means iff the node's label is in the command-line label set. */

bool prune_predicate(struct rnode *node, void *param)
{
	set_t *cl_labels = (set_t *) param;
	if (0 == strcmp("", node->label)) {
		fprintf(stderr, "empty label -> true.\n");
		return true;
	} else if (set_has_element(cl_labels, node->label)) {
		fprintf(stderr, "%s found -> true.\n", node->label);
		return true;
	} else {
		fprintf(stderr, "%s not found, ", node->label);
		if (is_inner_node(node)) {
			fprintf(stderr, "inner -> true\n");
			return true;
		} else {
			fprintf(stderr, "not inner -> false\n");
			return false;
		}
	}
}

static struct rooted_tree * process_tree_reverse(
		struct rooted_tree *tree, set_t *cl_labels)
{
	// TODO: do a "mark pass" through the tree to mark kept nodes and their
	// ancestors. Then use the mark in the predicate function.
	struct rooted_tree *pruned = clone_tree_cond(tree,
			prune_predicate, cl_labels);	
	//destroy_tree(tree);
	return pruned;
}

int main(int argc, char *argv[])
{
	struct rooted_tree *tree;	
	struct parameters params;
	static struct rooted_tree * (*process_tree)(struct rooted_tree *, set_t *);
	
	params = get_params(argc, argv);

	switch (params.mode) {
	case PRUNE_DIRECT:
		process_tree = process_tree_direct;
		break;
	case PRUNE_REVERSE:
		process_tree = process_tree_reverse;
		break;
	default:
		assert (0);
	}

	while (NULL != (tree = parse_tree())) {
		tree = process_tree(tree, params.cl_labels);
		dump_newick(tree->root);
		destroy_all_rnodes(NULL);
		destroy_tree(tree);
	}

	destroy_set(params.cl_labels);

	return 0;
}
