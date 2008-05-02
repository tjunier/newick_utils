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

extern int FREE_NODE_DATA;

enum {FROM_ROOT, FROM_LCA, MATRIX, FROM_PARENT};
enum {HORIZONTAL, VERTICAL};
enum {SQUARE, TRIANGLE};

struct parameters {
	struct llist *labels;
	int distance_type;
	char separator;
	int only_leaves;
	int header;
	int orientation;
	int shape;
};

/* Returns the distance type (root, LCA, or matrix) based on the first characer
 * of 'optarg' */

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
	params.separator = '\n';
	params.only_leaves = 1;
	params.header = 0;
	params.orientation = VERTICAL;
	params.shape = SQUARE;

	int opt_char;
	while ((opt_char = getopt(argc, argv, "him:t")) != -1) {
		switch (opt_char) {
		case 'm':
			params.distance_type = get_distance_type();
			break;
		case 't':
			params.orientation = HORIZONTAL;
			break;
		case 'h':
			params.header = 1;
			break;
		case 'i':
			params.only_leaves = 0;
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
	} else {
		fprintf(stderr, "Usage: %s [-ma] <filename|-> [label+]\n",
				argv[0]);
		exit(EXIT_FAILURE);
	}

	return params;
}

struct hash *distance_hash (struct rooted_tree *tree, struct rnode *origin,
		struct llist *labels)
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

void print_distance_list (struct rooted_tree *tree, struct rnode *origin,
		struct llist *labels, int orientation, int header)
{
	struct hash *lbl2dist = distance_hash(tree, origin, labels);

	struct list_elem *el;
	char *label;
	double *distance;
	if (VERTICAL == orientation) {
		for (el = labels->head; NULL != el; el = el->next) {
			label = el->data;
			distance = hash_get(lbl2dist, label);
			if (header) { printf("%s\t", label); }
			printf( "%g\n", *distance);
		}
	} else if (HORIZONTAL == orientation) {
		if (header) {
			for (el = labels->head; NULL != el; el = el->next) {
				label = el->data;
				if (el != labels->head) { putchar ('\t'); }
				printf( "%s", label);
			}
			putchar('\n');
		}
		for (el = labels->head; NULL != el; el = el->next) {
			label = el->data;
			distance = hash_get(lbl2dist, label);
			if (el != labels->head) { putchar ('\t'); }
			printf( "%g", *distance);
		}
		putchar('\n');
	} else {
		fprintf (stderr, "ERROR: unknown orientation (%d)\n", 
				orientation);
		destroy_hash(lbl2dist);
		exit(EXIT_FAILURE);
	}

	destroy_hash(lbl2dist);
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

double ** fill_matrix (struct rooted_tree *tree, struct llist *labels)
{
	struct list_elem *h_el, *v_el;
	int i, j;
	int count = labels->count;
	struct hash *lbl2node_map = create_label2node_map(tree->nodes_in_order);
	
	double **lines = malloc(count * sizeof(double *));
	if (NULL == lines) { perror(NULL), exit (EXIT_FAILURE); }
	
	for (j=0, v_el=labels->head; NULL != v_el; v_el=v_el->next, j++) {

		lines[j] = malloc(count * sizeof(double));
		if (NULL == lines[j]) { perror(NULL), exit (EXIT_FAILURE); }

		struct rnode *h_node, *v_node;
		v_node = hash_get(lbl2node_map, (char *) v_el->data);

		for (i=0,h_el=labels->head; NULL!=h_el; h_el=h_el->next,i++) {
			h_node = hash_get(lbl2node_map, (char *) h_el->data);
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

	destroy_hash(lbl2node_map);
	return lines;
}

void print_distance_matrix (struct rooted_tree *tree, struct llist *labels,
		int headers, int shape)
{
	double **matrix = fill_matrix(tree, labels);

	struct list_elem *h_el, *v_el;
	int i, j;
	
	for (j=0, v_el=labels->head; NULL != v_el; v_el=v_el->next, j++) {
		if (headers) printf ("%s\t", (char *) v_el->data);
		for (i=0,h_el=labels->head; NULL!=h_el; h_el=h_el->next,i++) {
			printf("%g", matrix[j][i]);
			if (h_el == labels->tail) 
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
		struct rnode *lca;
		struct llist *labels = params.labels;
		/* if no lbl given, use all leaf labels or all labels */
		if (0 == labels->count) 
			if (params.only_leaves)
				labels = get_leaf_labels(tree);
			else
				labels = get_labels(tree);
		switch (params.distance_type) {
		case FROM_ROOT:
			print_distance_list(tree, tree->root, labels, params.orientation, params.header);
			break;
		case FROM_LCA:
			/* if no lbl given, use root as LCA */
			if (0 == labels->count) 
				lca = tree->root;
			else
				lca = lca_from_labels(tree, labels);
			print_distance_list(tree, lca, labels,
				params.orientation, params.header);
			break;
		case MATRIX:
			print_distance_matrix(tree, labels, params.header,
					params.shape);
			break;
		case FROM_PARENT:
			print_distance_list(tree, NULL, labels,
				params.orientation, params.header);
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
