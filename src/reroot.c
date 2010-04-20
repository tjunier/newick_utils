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
/* reroot: reroot tree above specified node */
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>

#include "tree.h"
#include "parser.h"
#include "nodemap.h"
#include "to_newick.h"
#include "list.h"
#include "lca.h"
#include "rnode.h"
#include "hash.h"
#include "common.h"
#include "link.h"

enum reroot_status { REROOT_OK, LCA_IS_TREE_ROOT };
enum deroot_status { DEROOT_OK, BALANCED, NOT_BIFURCATING };

struct parameters {
	struct llist *labels;
	bool try_ingroup;
	bool deroot;
};

void help(char *argv[])
{
	printf (
"(Re)roots a tree on a specified outgroup\n"
"\n"
"Synopsis\n"
"--------\n"
"\n"
"%s [-hl] <newick trees filename|-> <label> [label*]\n"
"\n"
"Input\n"
"-----\n"
"\n"
"First argument is the name of a file that contains Newick trees, or '-' (in\n"
"which case trees are read from standard input).\n"
"\n"
"Further arguments are node labels. There must be at least one.\n"
"\n"
"Output\n"
"------\n"
"\n"
"Re-roots the tree on the outgroup formed by the nodes whose labels are\n"
"passed as arguments (by finding their LCA and rooting on its parent edge).\n"
"\n"
"Options\n"
"-------\n"
"\n"
"    -d: deroot - splice out the LCA of the ingroup, attaching its children\n"
"        to the root. The ingroup is the root's child which has the more\n"
"        children. The root is expected to have two children.\n"
"    -h: print this message and exit\n"
"    -l: lax - if it is not possible to reroot on the outgroup, try the\n"
"        ingroup - that is, all nodes whose labels were NOT passed as\n"
"        arguments.  This can also fail, if both the outgroup and the\n"
"        ingroup have the tree's root as LCA. Note that to use this option\n"
"        you must make sure that you pass ALL outgroup labels, otherwise the\n"
"        ingroup will be wrong.\n"
"\n"
"Examples\n"
"--------\n"
"\n"
"# This tree is rooted on humans, but the outgroup should be Cebus (New\n"
"# World monkey) - let's fix that: \n"
"\n"
"$ %s data/catarrhini_wrong Cebus\n"
"\n"
"# We can reroot on more than one node:\n"
"\n"
"$ %s data/catarrhini_wrong_3og Cebus Aotus \n",
	argv[0],
	argv[0],
	argv[0]
	);
}

struct parameters get_params(int argc, char *argv[])
{

	struct parameters params;

	params.try_ingroup = false;
	params.deroot = false;

	int opt_char;
	while ((opt_char = getopt(argc, argv, "dhl")) != -1) {
		switch (opt_char) {
		case '?':
			exit (EXIT_FAILURE);
		case 'd':
			params.deroot = true;
			break;
		case 'h':
			help(argv);
			exit(EXIT_SUCCESS);
		case 'l':
			params.try_ingroup = true;
			break;
		default:
			fprintf (stderr, "Unknown option '-%c'\n", opt_char);
			exit (EXIT_FAILURE);
		}
	}

	/* check arguments */
	int nargs = argc - optind; /* non-option arguments */
	int args_ok = true;
	switch (nargs) {
	case 0:
		args_ok = false;
		break;
	case 1:
		if (! params.deroot)
			args_ok = false;
	}
	if (! args_ok) {
		fprintf(stderr,
			"Usage: %s [-dhl] <filename|-> <label> [label+]\n",
			argv[0]);
		exit(EXIT_FAILURE);
	}
	/* read arguments */
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

	return params;
}

/* given the labels of the outgroup nodes, returns the nodes themselves, as a
 * llist. */

struct llist * get_outgroup_nodes(struct rooted_tree *tree, struct llist *labels)
{
	struct hash *map;
	struct llist *outgroup_nodes;
	struct list_elem *el;

	map = create_label2node_map(tree->nodes_in_order);	
	outgroup_nodes = create_llist();
	if (NULL == outgroup_nodes) { perror(NULL); exit(EXIT_FAILURE); }
	for (el = labels->head; NULL != el; el = el->next) {
		struct rnode *desc;
		desc = hash_get(map, (char *) el->data);
		if (NULL == desc) {
			fprintf (stderr, "WARNING: label '%s' does not occur in tree\n",
					(char *) el->data);
		} else {
			if (! append_element(outgroup_nodes, desc)) {
				perror(NULL);
				exit(EXIT_FAILURE);
			}
		}
	}
        destroy_hash(map);

	return outgroup_nodes;
}

/* Performs the rerooting itself. 'outgroup_nodes' usually means just that,
 * but may also mean ingroup nodes, if option -l was passed and rerooting on
 * outgroup failed. */

int reroot(struct rooted_tree *tree, struct llist *outgroup_nodes)
{
	struct rnode *outgroup_root;

	outgroup_root = lca(tree, outgroup_nodes);
	if (NULL == outgroup_root) { perror(NULL); exit(EXIT_FAILURE); }

	if (tree->root == outgroup_root) {
		return LCA_IS_TREE_ROOT;
	}
	if (! reroot_tree (tree, outgroup_root)) {
		perror(NULL);
		exit(EXIT_FAILURE);
	}

	return REROOT_OK;
}


/* De-roots a tree, in the sense that the top node must contain 3 (or more if
 * the tree isn't strictly bifurcating) children. */

enum deroot_status deroot(struct rooted_tree *tree)
{
	if (2 != tree->root->children->count)
		return NOT_BIFURCATING;
	struct rnode *left_kid = tree->root->children->head->data;
	struct rnode *right_kid = tree->root->children->tail->data;
	/* We splice out the left or right kid of the root, and also free() it. */
	if (left_kid->children->count < right_kid->children->count) {
		if (! splice_out_rnode(right_kid)) {
			perror(NULL); exit(EXIT_FAILURE);
		}
		// destroy_rnode(right_kid, NULL);
	}
	else if (left_kid->children->count > right_kid->children->count) {
		if (! splice_out_rnode(left_kid)) {
			perror(NULL); exit(EXIT_FAILURE);
		}
		// destroy_rnode(left_kid, NULL);
	}
	else 
		return BALANCED;

	return DEROOT_OK;
}

/* Returns true IFF arguments (cast to char*) are equal - IOW, the negation
 * of strcmp(). See llist_index_of() in list.h */

int string_eq(void *list_data, void *target)
{
	return (strcmp((char *) list_data, (char *) target) == 0);
}

/* Return a list of leaves whose labels are NOT found in
 * 'excluded_labels' */ 

struct llist *get_ingroup_leaves(struct rooted_tree *tree,
		struct llist *excluded_labels)
{
	struct llist *result = create_llist();
	if (NULL == result) { perror(NULL); exit(EXIT_FAILURE); }
	struct list_elem *el;

	/* add nodes to result iff i) node is a leaf, ii) node's label is not
	 * among 'excluded_labels' */ 
	for (el = tree->nodes_in_order->head; NULL != el; el = el->next) {
		struct rnode *current = (struct rnode *) el->data;
		if (is_leaf(current)) {
			/* Can't use llist_index_of(), because it compares the
			 * addresses of the 'data' members of elements. Instead
			 * we must check string equality, which is why we use
			 * llist_index_of_f(), and pass it string_eq(). */
			if (llist_index_of_f(excluded_labels, string_eq,
						current->label) == -1) 
				if (! append_element(result, current)) {
					perror(NULL);
					exit(EXIT_FAILURE);
				}
			
		}
	}

	return result;
}

void try_ingroup(struct rooted_tree *tree, struct parameters params)
{
	/* we will try to insert the root above the ingroup - for this we'll
	 * need all leaves that are NOT in the outgroup. */
	// TODO: why just leaves?
	struct llist *ingroup_leaves;
	ingroup_leaves = get_ingroup_leaves(tree, params.labels);
	enum reroot_status result = reroot(tree, ingroup_leaves);
	switch (result) {
		case REROOT_OK:
			dump_newick(tree->root);
			break;
		case LCA_IS_TREE_ROOT:
			fprintf (stderr, "LCA is still tree's root "
				"- be sure to include ALL outgroup "
				"leaves with -l");
			break;
	}
	destroy_llist(ingroup_leaves);
}

/* Tries to reroot 'directly', i.e. using outgroup. If this fails (because the
 * outgroup's LCA is the tree's root) and 'try_ingroup' is true (option -l),
 * tries with the ingroup */

void process_tree(struct rooted_tree *tree, struct parameters params)
{
	struct llist *outgroup_nodes = get_outgroup_nodes(tree, params.labels);
	if (! params.deroot) {
		/* re-root according to outgroup nodes */
		enum reroot_status result = reroot(tree, outgroup_nodes);
		switch (result) {
		case REROOT_OK:
			dump_newick(tree->root);
			break;
		case LCA_IS_TREE_ROOT:
			if (params.try_ingroup)
				try_ingroup(tree, params);
			else {
				fprintf (stderr,
					"Outgroup's LCA is tree's root "
					"- cannot reroot. Try -l.\n");
			}
			break;
		default:
			assert(0);
		}
		destroy_tree_cb(tree, NULL);
	} else {
		enum deroot_status result = deroot(tree);
		switch (result) {
		case DEROOT_OK:
			dump_newick(tree->root);
			break;
		case NOT_BIFURCATING:
			fprintf (stderr,
				"WARNING: tree is already unrooted, or root"
				" has only 1 child - cannot deroot.\n");
			break;
		case BALANCED:
			fprintf (stderr,
				"WARNING: can't decide which of root's "
				"children is the outgroup.\n");
			break;
		default:
			assert(0);
		}
		destroy_tree_cb(tree, NULL);
	}
	destroy_llist(outgroup_nodes);
}

int main(int argc, char *argv[])
{
	struct rooted_tree *tree;	
	struct parameters params;
	
	params = get_params(argc, argv);
	while (NULL != (tree = parse_tree())) {
		/* tree is free()d in process_tree(), as derooting is
		 * compatible with ordinary free()ing (with destroy_tree()),
		 * but rerooting is not. */
		process_tree(tree, params);
	}

	destroy_llist(params.labels);
	return 0;
}

