/* subtree: print subtree from a specified node */
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <regex.h>

#include "tree.h"
#include "parser.h"
#include "nodemap.h"
#include "hash.h"
#include "to_newick.h"
#include "list.h"
#include "lca.h"
#include "rnode_iterator.h"
#include "rnode.h"
#include "common.h"
#include "link.h"

enum {EXACT, REGEXP};

struct parameters {
	struct llist *labels;
	int check_monophyly;
	int siblings;
	int mode;
	char *regexp_string;
};

void help(char *argv[])
{
	printf(
"Extracts a subtree (clade) defined by labels.\n"
"\n"
"Synopsis\n"
"--------\n"
"\n"
"%s [-hmrs] <target tree filename|-> <label> [label]+\n"
"\n"
"Input\n"
"-----\n"
"\n"
"The first argument is the name of a file containing one or more Newick\n"
"trees, or '-' (in which case the trees are read on stdin).\n"
"\n"
"The next arguments are labels found in the tree (both leaf and internal\n"
"labels work). Any label not found in the tree will be ignored. There\n"
"must be at least one label. (See also option -r)\n"
"\n"
"Output\n"
"------\n"
"\n"
"Outputs the clade rooted at the last common ancestor of all labels passed\n"
"as arguments, as Newick.\n"
"\n"
"Options\n"
"-------\n"
"\n"
"    -h: prints this message and exits\n"
"    -m: only prints the clade if it is monophyletic, in the sense that ONLY\n"
"        the labels passed as arguments are found in the clade.\n"
"        See also -s.\n"
"    -r <regexp>: clade is defined by labels that match the regexp (instead.\n"
"        of labels passed as arguments)\n"
"    -s: prints the siblings of the clade defined by the labels passed as\n"
"        arguments, in the order in which they appear in the Newick.\n"
"        If -m is also passed, only prints siblings if the labels passed\n"
"        as arguments form a monophyletic group.\n"
"\n"
"Examples\n"
"--------\n"
"\n"
"# clade defined by two leaves\n"
"$ %s data/catarrhini Homo Hylobates\n"
"\n"
"# clade defined by a leaf and a inner node (Hominini)\n"
"$ %s data/catarrhini Hominini Hylobates\n"
"\n"
"# clade is monophyletic\n"
"$ %s -m data/catarrhini Homo Gorilla Pan\n"
"\n"
"# clade is not monophyletic (Gorilla is missing)\n"
"$ %s -m data/catarrhini Homo Pongo Pan\n"
"\n"
"# clade defined by a regexp (all Poliovirus)\n"
"$ %s -r data/HRV.nw '^POLIO.*'\n",
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

	params.check_monophyly = FALSE;
	params.siblings = FALSE;
	params.mode = EXACT;

	int opt_char;
	while ((opt_char = getopt(argc, argv, "hmrs")) != -1) {
		switch (opt_char) {
		case 'h':
			help(argv);
			exit(EXIT_SUCCESS);
		case 'm':
			params.check_monophyly = TRUE;
			break;
		case 'r':
			params.mode = REGEXP;
			break;
		case 's':
			params.siblings = TRUE;
			break;
		default:
			fprintf (stderr, "Unknown option '-%c'\n", opt_char);
			exit(EXIT_FAILURE);
			break; /* ok, not very useful... but I might later decide to ignore the bad option rather than fail. */
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
		struct llist *lbl_list;
		switch (params.mode) {
		case EXACT:
			lbl_list = create_llist();
			optind++;	/* optind is now index of 1st label */
			for (; optind < argc; optind++) {
				append_element(lbl_list, argv[optind]);
			}
			params.labels = lbl_list;
			break;
		case REGEXP:
			optind++;	/* optind is now index of regexp */
			params.regexp_string = argv[optind];
			break;
		default:
			fprintf (stderr, "Unknown mode %d\n", params.mode);
			exit(EXIT_FAILURE);
		}
	} else {
		fprintf(stderr, "Usage: %s [-hm] <filename|-> <label> [label+]\n",
				argv[0]);
		exit(EXIT_FAILURE);
	}

	return params;
}

int is_monophyletic(struct llist *descendants, struct rnode *subtree_root)
{
	int result = TRUE;

	struct hash *leaf_map = get_leaf_label_map(subtree_root);
	if (leaf_map->count != descendants->count) {
		result = FALSE;
	}
	struct list_elem *el;
	for (el = descendants->head; NULL != el; el = el->next) {
		char *label = ((struct rnode *) el->data)->label;
		if (NULL == hash_get(leaf_map, label)) {
			result = FALSE;
			break;
		}
	}

	destroy_hash(leaf_map);

	return result;
}

void process_tree(struct rooted_tree *tree, struct parameters params)
{
	struct llist *descendants;

	switch (params.mode) {
	case EXACT:
		descendants = nodes_from_labels(tree, params.labels);
		if (0 == descendants->count) {
			fprintf (stderr, "WARNING: no label matches.\n");
			/* I don't consider this a failure: it is just the case
			 * that the tree does not contain the specified labels.
			 * */
			exit(EXIT_SUCCESS);
		}
		break;
	case REGEXP:
		descendants = nodes_from_regexp(tree, params.regexp_string);
		if (0 == descendants->count) {
			fprintf (stderr, "WARNING: no match for regexp /%s/\n",
					params.regexp_string);
			exit(EXIT_SUCCESS); /** see above */
		}
		break;
	default:
		fprintf (stderr, "Unknown mode %d\n", params.mode);
		exit(EXIT_FAILURE);
	}

	/* We need a copy b/c lca() modifies its arg */
	struct llist *desc_clone = shallow_copy(descendants);
	struct rnode *subtree_root = lca(tree, desc_clone);
	free(desc_clone); /* elems freed in lca() */

	if (NULL != subtree_root) {
		if ((! params.check_monophyly) ||
		    (is_monophyletic(descendants, subtree_root))) {
			/* monophyly of input labels is verified or not
			 * requested */
			char *newick;
			if (params.siblings) {
				struct llist *sibs = siblings(subtree_root);
				struct list_elem *el;
				for (el=sibs->head;NULL!=el;el=el->next) {
					struct rnode *sib;
					sib = el->data;
					newick = to_newick(sib);
					printf ("%s\n", newick);
					free(newick);
				}
				destroy_llist(sibs);
			} else {
				/* normal operation: print clade defined by
				 * labels. */
				newick = to_newick(subtree_root);
				printf ("%s\n", newick);
				free(newick);
			}
		}
	} else {
		fprintf (stderr, "WARNING: LCA not found\n");
	}

	destroy_llist(descendants);

}

int main(int argc, char *argv[])
{
	struct rooted_tree *tree;	
	struct parameters params;
	
	params = get_params(argc, argv);

	while ((tree = parse_tree()) != NULL) {
		process_tree(tree, params);
		destroy_tree(tree, DONT_FREE_NODE_DATA);
	}

	if (EXACT == params.mode)
		destroy_llist(params.labels);

	return 0;
}
