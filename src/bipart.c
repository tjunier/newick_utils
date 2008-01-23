/* bipart.c: compute bipartitions */

#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "tree.h"
#include "parser.h"
#include "list.h"
#include "hash.h"
#include "rnode.h"
#include "node_set.h"

/* for dumping the hash. Called on each element of the bins, passed the 'value'
 * member of the key-value pair */

void bin_elem_dump(void *bin_data)
{
	printf ("Value: %d\n", *((int *) bin_data));
}

/* Creates a label -> ordinal number map.  Returns 0 if there was a problem
 * (such as a leaf without a label, or a non-unique label; returns 1 otherwise
 * */

int process_tree(struct rooted_tree *tree, struct hash *name2num)
{
	struct list_elem *el;
	int hash_build_result;

	/* create name->number map if it does not exist - i.e., first tree is
	 * reference. */
	if (NULL == name2num) {
		hash_build_result = build_name2num(tree, &name2num);
		if (hash_build_result != NS_OK)
			return hash_build_result;
	}
	// dump_hash(name2num, bin_elem_dump);
	for (el = tree->nodes_in_order->head; NULL != el; el = el->next) {
		struct rnode *current = (struct rnode *) el->data;
		struct llist* nodeset_list;
		if (is_leaf(current)) { continue; }
		nodeset_list = children_node_set_list(current, name2num,
				tree->nodes_in_order->count);
	}
	return NS_OK;
}

void get_params(int argc, char *argv[])
{
	/*
	int opt_char;
	while ((opt_char = getopt(argc, argv, "l")) != -1) {
		switch (opt_char) {
		case 'l':
			params.try_ingroup = 1;
			break;
		default:
			fprintf (stderr, "Unknown option '-%c'\n", opt_char);
			exit (EXIT_FAILURE);
		}
	}
	*/

	/* check arguments */
	if ((argc - optind) == 1)	{
		if (0 != strcmp("-", argv[optind])) {
			FILE *fin = fopen(argv[optind], "r");
			extern FILE *yyin;
			if (NULL == fin) {
				perror(NULL);
				exit(EXIT_FAILURE);
			}
			yyin = fin;
		}
		struct llist *lbl_list = create_llist();
		optind++;	/* optind is now index of 1st label */
		for (; optind < argc; optind++) {
			append_element(lbl_list, argv[optind]);
		}
	} else {
		fprintf(stderr, "Usage: %s <filename|->\n",
				argv[0]);
		exit(EXIT_FAILURE);
	}
}

int main(int argc, char *argv[])
{
	struct rooted_tree *tree;	
	struct hash *name2num = NULL;

	get_params(argc, argv);
	
	while (NULL != (tree = parse_tree())) {
		int result = process_tree(tree, name2num);
		switch (result) {
			case NS_OK:
				break;
			case NS_DUP_LABEL:
				fprintf (stderr,
					"Labels are not unique\n");
				exit (EXIT_FAILURE);
				break;
			case NS_EMPTY_LABEL:
				fprintf (stderr, "Found empty label");
				exit (EXIT_FAILURE);
				break;
			default:
				/* programmer error, not user error */
				assert(0);
		}
	}

	return 0;
}
