/* reroot: reroot tree above specified node */
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "tree.h"
#include "parser.h"
#include "nodemap.h"
#include "to_newick.h"
#include "list.h"
#include "lca.h"
#include "rnode.h"
#include "hash.h"

/* return values for reroot() */

const int OK = 0;
const int NO_LCA = 1;
const int LCA_IS_TREE_ROOT = 2;

struct parameters {
	struct llist *labels;
	int try_ingroup;
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
"%s data/catarrhini_wrong Cebus\n"
"\n"
"# We can reroot on more than one node:\n"
"\n"
"%s data/catarrhini_wrong_3og Cebus Aotus \n",
	argv[0],
	argv[0],
	argv[0]
	);
}

struct parameters get_params(int argc, char *argv[])
{

	struct parameters params;

	params.try_ingroup = 0;

	int opt_char;
	while ((opt_char = getopt(argc, argv, "hl")) != -1) {
		switch (opt_char) {
		case 'h':
			help(argv);
			exit(EXIT_SUCCESS);
		case 'l':
			params.try_ingroup = 1;
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
		optind++;	/* optind is now index of 1st label */
		for (; optind < argc; optind++) {
			append_element(lbl_list, argv[optind]);
		}
		params.labels = lbl_list;
	} else {
		fprintf(stderr, "Usage: %s [-hl] <filename|-> <label> [label+]\n",
				argv[0]);
		exit(EXIT_FAILURE);
	}

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
	for (el = labels->head; NULL != el; el = el->next) {
		struct rnode *desc;
		desc = hash_get(map, (char *) el->data);
		if (NULL == desc) {
			fprintf (stderr, "WARNING: label '%s' does not occur in tree\n",
					(char *) el->data);
		} else {
			append_element(outgroup_nodes, desc);
		}
	}

	return outgroup_nodes;
}

/* Performs the rerooting itself. 'outgroup_nodes' usually means just that,
 * but may also mean ingroup nodes, if option -l was passed and rerooting on
 * outgroup failed. */

int reroot(struct rooted_tree *tree, struct llist *outgroup_nodes)
{
	struct rnode *outgroup_root;

	outgroup_root = lca(tree, outgroup_nodes);
	if (NULL != outgroup_root) {
		if (tree->root == outgroup_root) {
			return LCA_IS_TREE_ROOT;
		}
		reroot_tree (tree, outgroup_root);
		printf ("%s\n", to_newick(tree->root));
		return OK;
	} else {
		fprintf (stderr, "Could not find LCA.\n");
		return NO_LCA;
	}
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
	struct list_elem *el;

	/* add nodes to result iff i) node is a leaf, ii) node's label is not among
	 * 'excluded_labels' */ 
	for (el = tree->nodes_in_order->head; NULL != el; el = el->next) {
		struct rnode *current = (struct rnode *) el->data;
		if (is_leaf(current)) {
			/* Can't use llist_index_of(), because it compares the addresses of
			 * the 'data' members of elements. Instead we must check string
			 * equality, which is why we use llist_index_of_f(), and pass it
			 * string_eq(). */
			if (llist_index_of_f(excluded_labels, string_eq,
						current->label) == -1) {
				append_element(result, current);
			}
		}
	}

	return result;
}

/* Tries to reroot 'directly', i.e. using outgroup. If this fails (because the
 * outgroup's LCA is the tree's root) and 'try_ingroup' is true (option -l),
 * tries with the ingroup */

void process_tree(struct rooted_tree *tree, struct parameters params)
{
	struct llist *outgroup_nodes = get_outgroup_nodes(tree, params.labels);
	int result = reroot(tree, outgroup_nodes);
	if (LCA_IS_TREE_ROOT == result) {
		if (params.try_ingroup) {
			/* we will try to insert the root above the
			 * ingroup - for this we'll need all leaves
			 * that are NOT in the outgroup. */
			struct llist *ingroup_leaves;
			ingroup_leaves = get_ingroup_leaves(tree, params.labels);
			result = reroot(tree, ingroup_leaves);
			if (LCA_IS_TREE_ROOT == result) {
				fprintf (stderr, "LCA is still tree's root - be sure to include ALL ougroup leaves with -l");
			}
			destroy_llist(ingroup_leaves);
		}
		else {
			fprintf (stderr, "Outgroup's LCA is tree's root - cannot reroot. Try -l.\n");
		}
	}
	destroy_llist(outgroup_nodes);
}

int main(int argc, char *argv[])
{
	struct rooted_tree *tree;	
	struct parameters params;
	
	params = get_params(argc, argv);

	while (NULL != (tree = parse_tree())) {
		process_tree(tree, params);
	}

	return 0;
}
