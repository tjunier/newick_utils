/* distance: print distances between nodes, in various ways. */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "tree.h"
#include "parser.h"
#include "nodemap.h"
#include "hash.h"
#include "list.h"
#include "lca.h"
#include "simple_node_pos.h"
#include "rnode.h"
#include "redge.h"
#include "node_pos_alloc.h"
#include "common.h"

enum {FROM_ROOT, FROM_LCA, MATRIX, FROM_PARENT};
enum {HORIZONTAL, VERTICAL};
enum {SQUARE, TRIANGLE};
enum {ALL_NODES, ALL_LABELS, ALL_LEAF_LABELS, ARGV_LABELS};

struct parameters {
	int distance_type;
	int selection;
	struct llist *labels;
	char separator;
	int show_header;
	int list_orientation;
	int matrix_shape;
};

/* Returns the distance type (root, LCA, or matrix) based on the first characer
 * of 'optarg' */

void help(char *argv[])
{
	printf (
"%s <tree file|-> [label]*\n"
"\n"
"Prints distances between nodes, following tree branches, in various ways.\n"
"\n"
"Input\n"
"\n"
"First argument is the name of a file containing one or more trees, or '-'\n"
"(in which case trees are read from standard input). Any subsequent arguments\n"
"are node labels.\n"
"\n"
"Output\n"
"\n"
"By default, works in \"root\" mode, i.e. outputs distances from the tree's\n"
"root to all nodes whose labels are passed on the command line, preserving\n"
"order.  Each distance appears on a separate line. If no labels are passed,\n"
"behaves as if all leaf labels had been passed, in the order in which they\n"
"appear in the Newick.\n"
"\n"
"In \"last common ancestor\" (LCA) mode (see options), finds the LCA of all\n"
"nodes whose labels are passed on the command line; then behaves like root\n"
"mode, but using the LCA instead of the tree's root (which may be the same).\n"
"If no labels are passed, behaves as in root mode.\n"
"\n"
"In \"parent\" mode (see options), prints the distance between each node whose\n"
"label is found on the command line and its immediate parent, preserving\n"
"order.  Each distance appears on a separate line.  If no label is passed,\n"
"behaves as if all leaf labels had been passed, in the order in which they\n"
"appear in the Newick.\n"
"\n"
"In \"matrix\" mode (see options), outputs a square matrix of distances between\n"
"all nodes whose labels are passed on the command line, preserving order. If\n"
"no labels are passed, behaves as if all leaf labels had been passed, in the\n"
"order in which they appear in the Newick.\n"
"\n"
"Options\n"
"\n"
"    -h: prints this message and exit \n"
"    -i: if no labels have been passed on the command line, behaves as if\n"
"        internal labels had been passed too (default: only leaf labels)\n"
"    -m <mode>: selects mode (see Output). Mode is determined by the first\n"
"        letter of the argument: 'r' for root mofr (default), 'l' for LCA,\n"
"        'p' for parent, and 'm' for matrix. Thus, '-mm', '-m matrix',\n"
"        and '-m mat' all select matrix mode.\n"
"    -n: prints labels as well as distances\n"
"    -t: tab-separated - prints values on one line, separated by tabs.\n"
"        Ignored in matrix mode.\n"
"\n"
"Assumptions and Limitations\n"
"\n"
"Labels passed as arguments are assumed to exist in the tree. Behaviour is\n"
"undefined if a label is not found.\n"
"\n"
"Examples\n"
"\n"
"$ %s data/catarrhini	# all leaves, from root\n"
"\n"
"# Pongo and Homo, from their LCA, with labels, single line:\n"
"$ %s -m lca -t -n data/catarrhini Homo Pongo \n"
"\n"
"# All leaves, matrix form, with labels\n"
"$ %s -mm -n data/catarrhini\n",
	argv[0],
	argv[0],
	argv[0],
	argv[0]);
}

int get_distance_type()
{
	switch (tolower(optarg[0])) {
	case 'l': /* lca, l, etc */
		return FROM_LCA;
	case 'm': /* matrix, m, etc */ 
		return MATRIX;
	case 'r': /* root, r, etc - default anyway */ 
		return FROM_ROOT;
	case 'p':
		return FROM_PARENT;
	default:
		fprintf (stderr, 
			"ERROR: unknown distance method '%s'\nvalid values: l(ca), m(atrix), r(oot), p(arent)", optarg);
		exit(EXIT_FAILURE);
	}
	/* should never get here */
	return -1;
}

struct parameters get_params(int argc, char *argv[])
{

	struct parameters params;

	params.distance_type = FROM_ROOT;
	params.selection = ALL_LEAF_LABELS;
	params.separator = '\n';
	params.show_header = FALSE;
	params.list_orientation = VERTICAL;
	params.matrix_shape = SQUARE;

	int opt_char;
	while ((opt_char = getopt(argc, argv, "ahim:nt")) != -1) {
		switch (opt_char) {
		case 'a':
			params.selection = ALL_NODES;
		case 'h':
			help(argv);
			exit(EXIT_SUCCESS);
		case 'i':
			params.selection = ALL_LABELS;
			break;
		case 'm':
			params.distance_type = get_distance_type();
			break;
		case 'n':
			params.show_header = TRUE;
			break;
		case 't':
			params.list_orientation = HORIZONTAL;
			break;
		default:
			fprintf (stderr, "Unknown option '-%c'\n", opt_char);
			exit(EXIT_FAILURE);
			break; /* ok, not very useful... but I might later decide to ignore the bad option rather than fail. */
		}
	}

	/* check arguments */
	if ((argc - optind) >= 1)	{
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
		if (0 != lbl_list->count)
			params.selection = ARGV_LABELS;
	} else {
		fprintf(stderr, "Usage: %s [-ma] <filename|-> [label+]\n",
				argv[0]);
		exit(EXIT_FAILURE);
	}

	return params;
}

struct llist *get_selected_nodes (struct rooted_tree *tree, 
		int selection)
{
	struct llist *result;
	struct list_elem *el;
	struct rnode *node;

	switch (selection) {
		case ALL_NODES:
			result = shallow_copy(tree->nodes_in_order);
			break;
		case ALL_LABELS:
			result = create_llist();
			for (el = tree->nodes_in_order->head; NULL != el;
				el = el->next) {
				node = el->data;
				if (0 != strcmp(node->label, ""))
					append_element(result, node);
			}
			break;
		case ALL_LEAF_LABELS:
			result = create_llist();
			for (el = tree->nodes_in_order->head; NULL != el;
				el = el->next) {
				node = el->data;
				if (is_leaf(node) &&
				   (0 != strcmp(node->label, "")))
					append_element(result, node);
			}
			break;
		default:
			fprintf (stderr, "ERROR: no selection code '%d'\n",
					selection);
			exit (EXIT_FAILURE);
	}

	return result;
}

/*TODO: if this function isn't used, drop it. */

/*
struct hash *distance_hash (struct rooted_tree *tree, struct rnode *origin,
		struct llist *selected_nodes)
{

	double origin_depth;
	if (NULL != origin) 
		origin_depth = ((struct simple_node_pos *) origin->data)->depth;

	struct hash *node_map = create_label2node_map(tree->nodes_in_order);
	struct hash *distances = create_hash(labels->count);
	struct list_elem *el;
	for (el = labels->head; NULL != el; el = el->next) {
		char *label = (char *) el->data;
		if (0 == strcmp("", label)) {
			fprintf(stderr, "WARNING: empty label.\n");
			continue;
		}
		struct rnode *node = hash_get(node_map, label);
		if (NULL == node) {
			fprintf(stderr, "WARNING: no node with label '%s'.\n",
					label);
			continue;
		}
		double node_depth = ((struct simple_node_pos *) node->data)->depth;
		if (NULL == origin) {
			struct rnode *parent = node->parent_edge->parent_node;
			origin_depth = ((struct simple_node_pos *) parent->data)->depth;
		}
		double *distance = malloc(sizeof(double));
		if (NULL == distance) { perror(NULL); exit(EXIT_FAILURE); }
		*distance =  node_depth - origin_depth;
		hash_set(distances, label, distance);
	}

	return distances;
}
*/

/* Returns the distance from node 'ancestor' to node 'descendant', where
 * 'descendant' is a decendant of 'ancestor'. Node positions must have been
 * assigned in simple_node_pos structures. */

double distance_to_descendant(struct rnode *ancestor, struct rnode *descendant)
{
	if (NULL == ancestor) {
		ancestor = descendant->parent_edge->parent_node;
	}

	double ancestor_depth =
		((struct simple_node_pos *) ancestor->data)->depth;
	double descendant_depth =
		((struct simple_node_pos *) descendant->data)->depth;
	
	return descendant_depth - ancestor_depth;
}

void print_distance_list (struct rooted_tree *tree, struct rnode *origin,
		struct llist *selected_nodes, int orientation, int header)
{
	struct list_elem *el;
	struct rnode *node;
	double distance;
	if (VERTICAL == orientation) {
		for (el = selected_nodes->head; NULL != el; el = el->next) {
			node = el->data;
			distance = distance_to_descendant(origin, node);
			if (header) { printf("%s\t", node->label); }
			printf( "%g\n", distance);
		}
	} else if (HORIZONTAL == orientation) {
		if (header) {
			for (el=selected_nodes->head; NULL!=el; el=el->next) {
				node = el->data;
				if (el != selected_nodes->head)
					putchar ('\t');
				printf( "%s", node->label);
			}
			putchar('\n');
		}
		for (el = selected_nodes->head; NULL != el; el = el->next) {
			node = el->data;
			distance = distance_to_descendant(origin, node);
			if (el != selected_nodes->head) { putchar ('\t'); }
			printf( "%g", distance);
		}
		putchar('\n');
	} else {
		fprintf (stderr, "ERROR: unknown orientation (%d)\n", 
				orientation);
		exit(EXIT_FAILURE);
	}
}

/* NOTE: this function could be made more efficient. First, the matrix is
 * symmetric, yet each cell is computed. It is trivial to halve the job.
 * Second, for every pait of labels, the LCA is computed from scratch. But
 * there is a better way, akin to dynamic programming (heck, maybe it *is*
 * dynamic programming): one can fill a table of LCAs in the following way:
 *
 * 1. for each inner node, in parse order:
 * 	1.1. store its leftmost and rightmost descendants's labels (leftmost
 * 	descendant is leftmost descendant of its left child, rightmost
 * 	descendant is rightmost descendant of its right child)
 * 	1.2. in the matrix, find the cell (L,R), where L is the leftmost
 * 	descendants's label and R the rightmost descendant's. Set this cell's
 * 	value to the current inner node - it is the LCA of L and R.
 * 	1.3 now find all cells "below" this one, i.e. that are to the left or
 * 	below this one in the upper-right triangle (or to the right and above
 * 	in the lower-left triangle): these cells are set to the same value,
 * 	UNLESS they are already set.
 *
 * Of course this only works if the matrix is ordered in parse order too.
 * Moreover it only works when seeking the LCA of two leaves, but if one of the
 * descendants whose LCA we seek is an inner node, one can always fetch its
 * leftmost (or rightmost) descendant from the tree, and use this to get the
 * LCA.
 */

double ** fill_matrix (struct rooted_tree *tree, struct llist *selected_nodes)
{
	struct list_elem *h_el, *v_el;
	int i, j;
	int count = selected_nodes->count;
	
	double **lines = malloc(count * sizeof(double *));
	if (NULL == lines) { perror(NULL), exit (EXIT_FAILURE); }
	
	for (j = 0, v_el = selected_nodes->head; NULL != v_el;
		v_el = v_el->next, j++) {

		lines[j] = malloc(count * sizeof(double));
		if (NULL == lines[j]) { perror(NULL), exit (EXIT_FAILURE); }

		struct rnode *h_node, *v_node;
		v_node = v_el->data;

		for (i = 0, h_el = selected_nodes->head; NULL != h_el;
			h_el = h_el->next, i++) {

			h_node =  h_el->data;
			if (NULL == v_node || NULL == h_node) {
				lines[j][i] = -1;
				break;
			}
			struct rnode *lca = lca2(tree, h_node, v_node);
			lines[j][i] = 
				((struct simple_node_pos *) h_node->data)->depth
				+
				((struct simple_node_pos *) v_node->data)->depth
				-
				2 * ((struct simple_node_pos *) lca->data)->depth
				;
		}
	}	

	return lines;
}

void print_distance_matrix (struct rooted_tree *tree,
		struct llist *selected_nodes, int show_headers, int shape)
{
	double **matrix = fill_matrix(tree, selected_nodes);

	struct list_elem *h_el, *v_el;
	int i, j;
	
	for (j = 0, v_el = selected_nodes->head; NULL != v_el;
		v_el = v_el->next, j++) {

		if (show_headers) printf ("%s\t",
			((struct rnode *) v_el->data)->label);

		for (i = 0, h_el = selected_nodes->head; NULL != h_el;
			h_el = h_el->next , i++) {

			printf("%g", matrix[j][i]);
			if (h_el == selected_nodes->tail) 
				putchar('\n');
			else
				putchar('\t');
		}
	}	
}

int main(int argc, char *argv[])
{
	struct rooted_tree *tree;	
	struct parameters params;
	
	params = get_params(argc, argv);

	/* TODO: could take the switch out of the loop, since the distance type
	 * is fixed for the program's lifetime. OTOH the code is easier to
	 * understand this way, and it's unlikely the switch has a visible
	 * impact on performance. */

	while ((tree = parse_tree()) != NULL) {
		alloc_simple_node_pos(tree);
		set_node_depth_cb(tree,
				set_simple_node_pos_depth,
				get_simple_node_pos_depth);
		struct rnode *lca_node;
		struct llist *selected_nodes;

		if (ARGV_LABELS == params.selection) {
			selected_nodes = nodes_from_labels(tree,
					params.labels);
		} else {
			selected_nodes = get_selected_nodes(tree,
					params.selection);
		}
		switch (params.distance_type) {
		case FROM_ROOT:
			print_distance_list(tree, tree->root, selected_nodes,
				params.list_orientation, params.show_header);
			break;
		case FROM_LCA:
			/* if no lbl given, use root as LCA */
			if (0 == params.labels->count)  {
				lca_node = tree->root;
			}
			else {
				lca_node = lca_from_nodes(tree, selected_nodes);
			}
			print_distance_list(tree, lca_node, selected_nodes,
				params.list_orientation, params.show_header);
			break;
		case MATRIX:
			print_distance_matrix(tree, selected_nodes,
				params.show_header, params.matrix_shape);
			break;
		case FROM_PARENT:
			print_distance_list(tree, NULL, selected_nodes,
				params.list_orientation, params.show_header);
			break;
		default:
			fprintf (stderr,
				"ERROR: invalid distance type '%d'.\n",
				params.distance_type);
			exit(EXIT_FAILURE);
		}

		destroy_tree(tree, FREE_NODE_DATA);
	}

	destroy_llist(params.labels);

	return 0;
}
