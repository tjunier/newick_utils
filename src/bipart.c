
/* reroot: reroot tree above specified node */
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "tree.h"
#include "parser.h"
#include "nodemap.h"
#include "to_newick.h"
#include "list.h"
#include "hash.h"
#include "rnode.h"

enum bn2m_return {OK, DUP_LABEL, EMPTY_LABEL};

/* for dumping the hash. Called on each element of the bins, passed the 'value'
 * member of the key-value pair */

void bin_elem_dump(void *bin_data)
{
	printf ("Value: %d\n", *((int *) bin_data));
}

/* Creates a label -> ordinal number map.  Returns 0 if there was a problem
 * (such as a leaf without a label, or a non-unique label; returns 1 otherwise
 * */

int build_name2num(struct rooted_tree *tree, struct hash **name2num_ptr)
{
	/* If the tree is dichotomous and has N nodes, then it has L = (N+1)/2
	 * leaves. But for highly polytomous trees, L is higher, and can
	 * approach N in some cases (e.g. when most leaves are attached to the
	 * root due to low bootstrap support). So we allocate N bins. */
	struct hash *n2n = create_hash(tree->nodes_in_order->count);
	struct list_elem *el;
	int ord_number = 0;

	for (el = tree->nodes_in_order->head; NULL != el; el = el->next) {
		struct rnode *current = (struct rnode *) el->data;
		if (is_leaf(current)) {
			int *nump;
			if (strcmp("", current->label) == 0)
				return EMPTY_LABEL;
			if (NULL != hash_get(n2n, current->label)) 
				return DUP_LABEL;
			nump = malloc(sizeof(int));
			if (NULL == nump) {
				perror(NULL);
				exit(EXIT_FAILURE);
			}
			*nump = ord_number;
			hash_set(n2n, current->label, nump);
			ord_number++;
		}
	}	
	*name2num_ptr = n2n;

	return OK;
}

int process_tree(struct rooted_tree *tree, struct hash *name2num)
{
	struct list_elem *el;
	int hash_build_result;

	/* create name->number map if it does not exist - i.e., first tree is
	 * reference. */
	if (NULL == name2num) {
		hash_build_result = build_name2num(tree, &name2num);
		if (hash_build_result != OK)
			return hash_build_result;
	}
	dump_hash(name2num, bin_elem_dump);
	if 
	/* attribute a number to all leaf nodes, so that numbering is conserved
	 * across trees, i.e. the same leaf will have the same number */
	attribute_number_to_leaves(tree, name2num);
	for (el = tree->nodes_in_order->head; NULL != el; el = el->next) {
		struct rnode *current = (struct rnode *) el->data;
		if (is_leaf(current)) {
			printf ("%s -> %d\n", current->label,
				*((int *) current->data));
		}
	}
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
			case OK:
				break;
			case DUP_LABEL:
				fprintf (stderr,
					"Labels are not unique\n");
				exit (EXIT_FAILURE);
				break;
			case EMPTY_LABEL:
				fprintf (stderr, "Found empty label");
				exit (EXIT_FAILURE);
				break;
			default:
				assert(0);
		}
	}

	return 0;
}
