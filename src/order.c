/* order.c - orders children nodes */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tree.h"
#include "parser.h"
#include "to_newick.h"
#include "list.h"
#include "rnode.h"
#include "redge.h"

void help (char *argv[])
{
	printf (
"Orders nodes by alphabetical order, preserving topology\n"
"\n"
"Synopsis\n"
"--------\n"
"\n"
"%s [-h] <newick trees filename|->\n"
"\n"
"Input\n"
"-----\n"
"\n"
"Argument is the name of a file that contains Newick trees, or '-' (in\n"
"which case trees are read from standard input).\n"
"\n"
"Output\n"
"------\n"
"\n"
"Orders the tree and prints it out on standard output. The ordering field\n"
"is the node's label for leaves, or the first child's order field for inner\n"
"nodes. The tree's topology is not altered: the biological information\n"
"contained in the tree is left intact.\n"
"\n"
"This is useful for comparing trees, because isomorphic trees will yield\n"
"different Newick representations if the nodes are ordered differently.\n"
"\n"
"Options\n"
"-------\n"
"\n"
"    -h: print this message and exit\n"
"\n"
"Examples\n"
"--------\n"
"\n"
"# These two trees look different...\n"
"$ diff -q data/falconiformes data/falconiformes_2 \n"
"Files data/falconiformes and data/falconiformes_2 differ\n"
"\n"
"# but are they really?\n"
"$ %s data/falconiformes > f1\n"
"$ %s data/falconiformes_2 > f2\n"
"\n"
"# No, they're in fact the same\n"
"$ diff -s f1 f2\n"
"Files f1 and f2 are identical\n",
	argv[0],
	argv[0],
	argv[0]
	       );
}

void get_params(int argc, char *argv[])
{

	int opt_char;
	while ((opt_char = getopt(argc, argv, "h")) != -1) {
		switch (opt_char) {
		case 'h':
			help(argv);
			exit(EXIT_SUCCESS);
		default:
			fprintf (stderr, "Unknown option '-%c'\n", opt_char);
			exit (EXIT_FAILURE);
		}
	}

	/* check arguments */
	if ((argc - optind) == 1)	{
		if (0 != strcmp("-", argv[optind])) {
			FILE *fin = fopen(argv[optind], "r");
			extern FILE *nwsin;
			if (NULL == fin) {
				perror(NULL);
				exit(EXIT_FAILURE);
			}
			nwsin = fin;
		}
	} else {
		fprintf(stderr, "Usage: %s [-h] <filename|->\n", argv[0]);
		exit(EXIT_FAILURE);
	}
}

int lbl_comparator(const void *a, const void *b)
{
	/* I really have trouble understanding how qsort() passes the
	 * comparands to the comparator... but thanks to GDB I figured out this
	 * one. */	
	char *a_lbl = (*(struct redge **)a)->child_node->data;
	char *b_lbl = (*(struct redge **)b)->child_node->data;

	int cmp = strcmp(a_lbl, b_lbl);
	// printf ("%s <=> %s: %d\n", a_lbl, b_lbl, cmp);

	return cmp;
}

void process_tree(struct rooted_tree *tree)
{
	struct list_elem *elem;

	/* the rnode->data member is used to store the sort field. For leaves,
	 * this is just the label; for inner nodes it is the sort field of the
	 * first child (after sorting). */

	for (elem=tree->nodes_in_order->head; NULL!=elem; elem=elem->next) {
		struct rnode *current = elem->data;
		if (is_leaf(current)) {
			current->data = current->label;
		} else {
			/* Since all children have been visited (because we're
			 * traversing the tree in parse order), we can just
			 * order the children on their sort field. */

			struct redge ** kids_array;
			int count = current->children->count;
			kids_array = (struct redge **)
				llist_to_array(current->children);
			destroy_llist(current->children);
			qsort(kids_array, count, sizeof(struct redge *),
					lbl_comparator);
			struct llist *ordered_kids_list;
			ordered_kids_list = array_to_llist(
				(void **) kids_array, count);
			current->children = ordered_kids_list;

			current->data = kids_array[0]->child_node->data;
		}
	}
}

int main(int argc, char *argv[])
{
	struct rooted_tree *tree;	
	
	get_params(argc, argv);

	while (NULL != (tree = parse_tree())) {
		process_tree(tree);
		char *newick = to_newick(tree->root);
		printf ("%s\n", newick);
		free(newick);
		destroy_tree(tree, DONT_FREE_NODE_DATA);
	}

	return 0;
}
