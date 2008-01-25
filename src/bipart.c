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
#include "redge.h"
#include "node_set.h"

/* for dumping the hash. Called on each element of the bins, passed the 'value'
 * member of the key-value pair */

void bin_elem_dump(void *bin_data)
{
	printf ("Value: %d\n", *((int *) bin_data));
}

/*
 * Constructs or retrieves the node set (= set of descendant leaves)
 * corresponding to each child of 'current', and returns them all as a list. If
 * a child is a leaf, its node set is constructed, using the 'lbl2num' map to
 * get the leaf's number from its label. If the child is not a leaf, then it
 * just retrieves its node set (which will have been computed earlier, since we
 * call this function on every inner node in parse order). 'node_count' is the
 * total number of nodes in any node set.  */

struct llist *children_node_set_list(struct rnode* current,
		struct hash *lbl2num, int node_count)
{
	struct llist *result = create_llist();
	struct list_elem *el;

	for (el = current->children->head; NULL != el; el = el->next) {
		struct rnode *child = ((struct redge *) el->data)->child_node;
		if (is_leaf(child)) {
			int *num = (int *) hash_get(lbl2num, child->label);
			node_set set = create_node_set(node_count);
			node_set_add(set, *num, node_count);
			append_element(result, set);
		} else {
			append_element(result, child->data);
		}
	}
	
	return result;	
}

void increment_count (struct hash *h, char *key)
{
	int * count = hash_get(h, key);
	if (NULL == count) {
		int * init = malloc(sizeof(int));
		if (NULL == init) {
			perror(NULL);
			exit (EXIT_FAILURE);
		}
		*init = 1;
		hash_set(h, key, init);
	} else {
		(*count)++;
		free(key);
	}
}

int process_tree(struct rooted_tree *tree, struct hash **name2num_ptr,
		struct hash **bipart_count_ptr)
{
	static int node_count; /* set on first tree */
	int hash_build_result;
	struct list_elem *el;

	/* First tree serves as reference for label->num map. All other trees
	 * use the same map so the same label always maps to the same node
	 * number. The bipartition counts hash is also created on the first
	 * iteration. */
	if (NULL == *name2num_ptr) {
		node_count = leaf_count(tree);
		/* the number of bipartitions is not larger than the number of
		 * leaves. */
		*bipart_count_ptr = create_hash(node_count);
		hash_build_result = build_name2num(tree, name2num_ptr);
		if (hash_build_result != NS_OK)
			return hash_build_result;
	}

	/* The main meat. Visits nodes in order, and for each node computes the
	 * union of the descendant leaves. */
	for (el = tree->nodes_in_order->head; NULL != el; el = el->next) {
		struct list_elem *el2;
		struct llist* nodeset_list;
		node_set all_desc_leaves;
		char * string_rep;

		struct rnode *current = (struct rnode *) el->data;
		if (is_leaf(current)) { continue; }
		nodeset_list = children_node_set_list(current, *name2num_ptr,
				node_count);
		all_desc_leaves = create_node_set(node_count);
		for (el2 = nodeset_list->head; NULL != el2; el2 = el2->next) {
			node_set set = (node_set) el2->data;
			all_desc_leaves = node_set_union(
					all_desc_leaves, set, node_count);
		}
		string_rep = node_set_to_s(all_desc_leaves, node_count);
		increment_count (*bipart_count_ptr, string_rep);
		/* now set this node's descendant leaf set to the union */
		current->data = all_desc_leaves;
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

void show_bipart_counts(struct hash *bipart_count) 
{
	struct llist *keys = hash_keys(bipart_count);
	struct list_elem *el;

	for (el = keys->head; NULL != el; el = el->next) {
		char *key = (char *) el->data;
		int *count = (int *) hash_get (bipart_count, key);
		printf ("%d\t%s\n", *count, key);
	}
}

int main(int argc, char *argv[])
{
	struct rooted_tree *tree;	
	struct hash *name2num = NULL;
	struct hash *bipart_count = NULL;

	get_params(argc, argv);
	
	while (NULL != (tree = parse_tree())) {
		int result = process_tree(tree, &name2num, &bipart_count);
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

	show_bipart_counts(bipart_count);

	return 0;
}
