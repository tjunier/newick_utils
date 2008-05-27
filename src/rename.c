/* reroot: rename tree nodes according to a map */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "tree.h"
#include "parser.h"
#include "to_newick.h"
#include "hash.h"
#include "list.h"
#include "rnode.h"
#include "readline.h"

struct parameters {
	char  *map_filename;
	int only_leaves;
};

void help(char *argv[])
{
	printf (
"Renames nodes using a mapping\n"
"\n"
"Synopsis\n"
"--------\n"
"\n"
"%s [-hl] <newick trees filename|-> <map filename>\n"
"\n"
"Input\n"
"-----\n"
"\n"
"First argument is the name of a file that contains Newick trees, or '-' (in\n"
"which case trees are read from standard input).\n"
"\n"
"Second argument is the name of the map file, which has one (old-name,\n"
"new-name) pair per line, e.g:\n"
"\n"
"cmp1	Compsognathus\n"
"trc	Triceratops\n"
"vlcr	Velociraptor\n"
"\n"
"Old and new names should be separated by whitespace.\n"
"\n"
"Output\n"
"------\n"
"\n"
"Prints the tree, after replacing all old names by the specified new name.\n"
"\n"
"Options\n"
"-------\n"
"\n"
"    -h: print this message and exit\n"
"    -l: only replace leaf labels. This is useful if all labels are numeric,\n"
"        but inner labels represent bootstraps, and you don't want to\n"
"        accidentally modify bootstrap values.\n"
"\n"
"Examples\n"
"--------\n"
"\n"
"# Some tree-building programs don't allow labels longer than 10 characters\n"
"# or so. One way around this limitation is to relabel the sequences using\n"
"# numbers.  Tree data/HRV_numeric has numeric labels, because its outgroup,\n"
"# \"Foot_and_Mouth\" is longer than 10 characters. The number->label mapping\n"
"# was stored in file data/HRV.map. Now we can rename the whole tree:\n"
"\n"
"%s data/HRV_numeric data/HRV.map\n"
"\n"
"# We can construct a tree of families from a genus tree and a genus->family\n"
"# map. File data/falconiformes is a tree of diurnal raptor genera, and\n"
"# data/falc_map maps the genera to families. To produce a family tree, we do: \n"
"\n"
"%s data/falconiformes data/falc_map\n"
"\n"
"# In fact, we could directly condense the tree, so that only one leaf per\n"
"# family is left:\n"
"\n"
"%s data/falconiformes data/falc_map | nw_condense -\n",
	argv[0],
	argv[0],
	argv[0],
	argv[0]
	);
}

struct hash *read_map(const char *filename)
{
	const int HASH_SIZE = 1000;	/* most trees will have fewer nodes */

	FILE *map_file = fopen(filename, "r");
	if (NULL == map_file) { perror(NULL); exit(EXIT_FAILURE); }

	struct hash *map = create_hash(HASH_SIZE);
	char *line;
	while (NULL != (line = read_line(map_file))) {
		char *key, *value;
		struct word_tokenizer *wtok = create_word_tokenizer(line);
		key = wt_next(wtok);	/* find first whitespace */
		if (NULL == key) {
			fprintf (stderr, "Invalid line format in map file %s: '%s'\n", filename, line);
			exit(EXIT_FAILURE);
		}
		value = wt_next(wtok);
		if (NULL == value) {
			fprintf (stderr, "Invalid line format in map file %s: '%s'\n", filename, line);
			exit(EXIT_FAILURE);
		}
		hash_set(map, key, (void *) value);
		destroy_word_tokenizer(wtok);
		free(key); /* copied by hash_set(), so can be free()d now */
		free(line);
	}

	return map;
}

struct parameters get_params(int argc, char *argv[])
{

	struct parameters params;

	params.only_leaves = 0;	/* default: rename all nodes */

	int opt_char;
	while ((opt_char = getopt(argc, argv, "hl")) != -1) {
		switch (opt_char) {
		case 'h':
			help(argv);
			exit(EXIT_SUCCESS);
		case 'l':
			params.only_leaves = 1;
			break;
		}
	}

	/* check arguments */
	if ((argc - optind) == 2)	{
		if (0 != strcmp("-", argv[optind])) {
			FILE *fin = fopen(argv[optind], "r");
			extern FILE *nwsin;
			if (NULL == fin) {
				perror(NULL);
				exit(EXIT_FAILURE);
			}
			nwsin = fin;
		}
		params.map_filename = argv[optind+1];
	} else {
		fprintf(stderr, "Usage: %s [-hl] <filename|-> <map_filename>\n",
				argv[0]);
		exit(EXIT_FAILURE);
	}

	return params;
}

void process_tree(struct rooted_tree *tree, struct hash *rename_map,
		struct parameters params)
{
	/* visit each node, and change name if needed */
	struct list_elem *elem;
	for (elem = tree->nodes_in_order->head; NULL != elem; elem = elem->next) {
		struct rnode *current = (struct rnode *) elem->data;
		if (params.only_leaves && ! is_leaf(current)) { continue; }
		char *label = current->label;
		char *new_label = hash_get(rename_map, label);
		if (NULL != new_label) {
			current->label = strdup(new_label);
			free(label);
		}
	}

	char *newick = to_newick(tree->root);
	printf ("%s\n", newick);
	free(newick);
}

int main(int argc, char *argv[])
{
	struct rooted_tree *tree;	
	struct hash *rename_map;
	struct parameters params;
	
	params = get_params(argc, argv);

	rename_map = read_map(params.map_filename);

	while (NULL != (tree = parse_tree())) {
		process_tree(tree, rename_map, params);
		destroy_tree(tree, DONT_FREE_NODE_DATA);
	}

	struct llist *keys = hash_keys(rename_map);
	struct list_elem *e;
	for (e = keys->head; NULL != e; e = e->next) {
		char *key = (char *) e->data;
		char *val = hash_get(rename_map, key);
		free(val);
	}
	destroy_llist(keys);
	destroy_hash(rename_map);

	return 0;
}
