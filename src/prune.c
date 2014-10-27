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
#include "readline.h"

enum prune_mode { PRUNE_DIRECT, PRUNE_REVERSE };
enum label_source { COMMAND_LINE, IN_FILE }; /* can't use FILE... */

struct prune_data {
	bool kept_descendant;
};

struct parameters {
	set_t 	*prune_labels;
	enum prune_mode mode;
	enum label_source lbl_src;
};

void help(char *argv[])
{
	printf (
"Removes nodes by label\n"
"\n"
"Synopsis\n"
"--------\n"
"\n"
"%s [-f:hv] <newick trees filename|-> <label> [label+]\n"
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
"    -f <filename>: node labels are in a file named <filename>, not on the\n"
"        command line. There should be one label per line.\n"
"    -h: print this message and exit\n"
"    -v: reverse: prune nodes whose labels are NOT passed on the command\n"
"        line. Inner nodes are not pruned. This allows pruning of trees\n"
"        with support values, which syntactically are node labels, withouti\n"
"        inner nodes disappearing because their 'label' was not passed on\n"
"        the command line.\n"
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
"$ %s -v data/catarrhini Hominidae Colobinae\n",
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
	const char *USAGE =
"Usage: nw_prune [-hv] <filename|-> <label> [label+]\n"
"or     nw_prune [-hv] -f <filename|-> <label_filename>";
	struct parameters params;
	params.mode = PRUNE_DIRECT;
	params.lbl_src = COMMAND_LINE;

	int opt_char;
	while ((opt_char = getopt(argc, argv, "fhv")) != -1) {
		switch (opt_char) {
		case 'f':
			params.lbl_src = IN_FILE;
			break;
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

	/* check and get arguments */

	/* get Newick input */

	if ((argc - optind) >= 1)	{
		if (0 != strcmp("-", argv[optind])) {
			FILE *fin = fopen(argv[optind], "r");
			extern FILE *nwsin;
			if (NULL == fin) {
				fprintf(stderr, "%s: ", argv[optind]);
				perror(NULL);
				exit(EXIT_FAILURE);
			}
			nwsin = fin;
		}
	}

	/* Get prune labels */

	set_t *prune_labels = create_set();
	if (NULL == prune_labels) { perror(NULL); exit(EXIT_FAILURE); }

	optind++;	
	if (COMMAND_LINE == params.lbl_src) {
		/* optind is now index of 1st label */
		if ((argc - optind) < 1) {
			fprintf(stderr, USAGE);
			exit(EXIT_FAILURE);
		}
		for (; optind < argc; optind++) {
			if (set_add(prune_labels, argv[optind]) < 0) {
				perror(NULL);
				exit(EXIT_FAILURE);
			}
		}
	} else if (IN_FILE == params.lbl_src) {
		/* optind is now index of label file name */
		if ((argc - optind) < 1) {
			fprintf(stderr, USAGE);
			fprintf(stderr, "optind = %d (should be >= 1)\n");
			exit(EXIT_FAILURE);
		} else if ((argc - optind) > 1) {
			fprintf (stderr, "WARNING: expecting two arguments, extra arguments will be ignored.\n");
		}
		FILE* lbl_src = fopen(argv[optind], "r");
		if (NULL == lbl_src) {
			fprintf(stderr, "%s: ", argv[optind]);
			perror(NULL);
			exit(EXIT_FAILURE);
		}
		char *label = NULL;
		while (NULL != (label = read_line(lbl_src))) {
			if (set_add(prune_labels, label) < 0) {
				perror(NULL);
				exit(EXIT_FAILURE);
			}
		}
		switch(read_line_status) {
		case READLINE_EOF:
			break;
		case READLINE_ERROR:
			perror(NULL);
			exit(EXIT_FAILURE);
		default:
			// Should never get here...
			assert(0);
		}


	} else {
		// Should never get here (programmer error...)
		assert(0);
	}

	params.prune_labels = prune_labels;
	return params;
}

/* We buld a hash of the passed labels, and visit the tree, unlinking nodes as
 * needed. In Direct mode, we traverse in reverse order, unlinking nodes to
 * prune. In Reverse mode, things are a bit more hairy because we need to keep
 * not just the nodes passed as argument, but also their ancestors. So we pass
 * through th etree first in direct order, marking nodes to keep and their
 * ancestors. Then we go back in reverse order, pruning. */

static struct rooted_tree * process_tree_direct(
		struct rooted_tree *tree, set_t *prune_labels)
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
			continue;
		}
		if (set_has_element(prune_labels, label)) {
			unlink_rnode(current);
			current->seen = true;
		}
	}

	destroy_llist(rev_nodes);
	reset_seen(tree);
	return tree;
}

/* Prune predicate: retains the nodes passed on CL, but discards their
 * children. */

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

// TODO: this might be a useful prune predicate, add option to use it.

bool prune_predicate_trim_kids(struct rnode *node, void *param)
{
	if (node->seen) {
		/* ancestor of a passed node */
		return true;
	}

	return false;
}

/* Prune predicate: retains the nodes passed on CL and their children. */

bool prune_predicate_keep_clade(struct rnode *node, void *param)
{
	// TODO: is this used?
	// set_t *prune_labels = (set_t *) param;

	if (node->seen) {
		return true;
	}
	/* Node isn't an ancestor of a passed node. Node can be a _descendant_
	 * of a passed node, though, in which case we must keep it as well. */
	if (!is_root(node)) {
		if (NULL != node->parent->data) {
			struct prune_data *pdata = node->parent->data;
			if (pdata->kept_descendant) {
				struct prune_data *pdata =
					malloc(sizeof(struct prune_data));
				if (NULL == pdata) {
					perror(NULL); exit(EXIT_FAILURE);
				}
				pdata->kept_descendant = true;
				node->data = pdata;
				return true;
			}
		}
	}
	
	/* neither an ancestor nor a descendant of a passed node - drop. */
	return false;
}

static struct rooted_tree * process_tree_reverse(
		struct rooted_tree *tree, set_t *prune_labels)
{
	struct list_elem *el = tree->nodes_in_order->head;
	struct rnode *current;
	char *label;

	for (; NULL != el; el = el->next) {
		current = el->data;
		if (is_root(current)) break;
		label = current->label;
		/* mark this node (to keep it) if its label is on the CL */
		if (set_has_element(prune_labels, label)) {
			current->seen = true;
			struct prune_data *pdata =
				malloc(sizeof(struct prune_data));
			if (NULL == pdata) {perror(NULL); exit(EXIT_FAILURE); }
			pdata->kept_descendant = true;
			current->data = pdata;
		}
		/* and propagate 'seen' to parent (kept_descendant is
		 * propagated to children (not parents), see
		 * prune_predicate_keep_clade() */
		if (current->seen) {
			current->parent->seen = true;	/* inherit mark */
		}
	}

	struct rooted_tree *pruned = clone_tree_cond(tree,
			prune_predicate_keep_clade, prune_labels);	
	destroy_tree(tree);
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
		tree = process_tree(tree, params.prune_labels);
		dump_newick(tree->root);
		destroy_all_rnodes(NULL);
		destroy_tree(tree);
	}

	destroy_set(params.prune_labels);

	return 0;
}

#pragma GCC diagnostic pop
