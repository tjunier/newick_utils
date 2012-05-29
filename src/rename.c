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
/* reroot: rename tree nodes according to a map */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <stdbool.h>

#include "tree.h"
#include "parser.h"
#include "to_newick.h"
#include "hash.h"
#include "list.h"
#include "rnode.h"
#include "readline.h"
#include "common.h"


struct parameters {
	/* Either uses a rename map (whose name is then stored in
	 * map_filename), or gets the old and new labels from the command line.
	 * This is determined from the number of arguments. */
	char *map_filename;
	char *old_label;
	char *new_label;
	bool only_leaves;
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
"or\n"
"%s [-hl] <newick trees filename|-> <old-label> <new-label>\n"
"\n"
"Input\n"
"-----\n"
"\n"
"First argument is the name of a file that contains Newick trees, or '-' (in\n"
"which case trees are read from standard input).\n"
"\n"
"In the first (two-argument) form, the second argument is the name of the\n"
"_map file_, which has one (old-name, new-name) pair per line, e.g:\n"
"\n"
"cmp1	Compsognathus\n"
"trc	Triceratops\n"
"hnr\n"
"vlcr	Velociraptor\n"
"\n"
"Old and new names should be separated by whitespace. If the new-name\n"
"is empty (such as for 'hnr' above), the label will be removed.\n"
"\n"
"In the second (three-argument) form, the second argument is the old label\n"
"(i.e., the one to be replaced), and the third argument is the replacement.\n"
"\n"
"In other words, the first form can replace more than one label, but\n"
"requires a map file, while the second form requires no file but is limited\n"
"to one label.\n"
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
"$ %s data/HRV_numeric data/HRV.map\n"
"\n"
"# We can construct a tree of families from a genus tree and a genus->family\n"
"# map. File data/falconiformes is a tree of diurnal raptor genera, and\n"
"# data/falc_map maps the genera to families. To produce a family tree, we do: \n"
"\n"
"$ %s data/falconiformes data/falc_map\n"
"\n"
"# In fact, we could directly condense the tree, so that only one leaf per\n"
"# family is left:\n"
"\n"
"$ %s data/falconiformes data/falc_map | nw_condense -\n",
	argv[0],
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
	if (NULL == map) { perror(NULL); exit(EXIT_FAILURE); }

	char *line;
	while (NULL != (line = read_line(map_file))) {
		/* Skip comments and lines that are empty or all whitespace */
		if ('#' == line[0] || is_all_whitespace(line)) {
			free(line);
			continue;
		}

		char *key, *value;
		struct word_tokenizer *wtok = create_word_tokenizer(line);
		if (NULL == wtok) { perror(NULL); exit(EXIT_FAILURE); }
		key = wt_next(wtok);	/* find first whitespace */
		if (NULL == key) {
			fprintf (stderr,
				"Wrong format in line '%s' - aborting.\n",
				line);
			exit(EXIT_FAILURE);
		}
		value = wt_next(wtok);
		if (NULL == value) {
			/* If 2nd token is NULL, replace label with empty
			 * string */
			value = strdup("");
		}
		if (! hash_set(map, key, (void *) value)) {
			perror(NULL);
			exit(EXIT_FAILURE);
		}
		destroy_word_tokenizer(wtok);
		free(key); /* copied by hash_set(), so can be free()d now */
		free(line);
	}

	return map;
}

struct parameters get_params(int argc, char *argv[])
{

	struct parameters params;

	params.only_leaves = false;	/* default: rename all nodes */
	params.map_filename = NULL;
	params.old_label = NULL;
	params.new_label = NULL;

	int opt_char;
	while ((opt_char = getopt(argc, argv, "hl")) != -1) {
		switch (opt_char) {
		case 'h':
			help(argv);
			exit(EXIT_SUCCESS);
		case 'l':
			params.only_leaves = true;
			break;
		}
	}

	/* check arguments */
	if ((argc - optind) < 2)	{
		fprintf(stderr, "Usage: %s [-hl] <filename|-> <map_filename>\n",
				argv[0]);
		exit(EXIT_FAILURE);
	} 

	if (0 != strcmp("-", argv[optind])) {
		FILE *fin = fopen(argv[optind], "r");
		extern FILE *nwsin;
		if (NULL == fin) {
			perror(NULL);
			exit(EXIT_FAILURE);
		}
		nwsin = fin;
	}
	if ((argc - optind) == 2)
		params.map_filename = argv[optind+1];
	else  {
		params.old_label = argv[optind+1];
		params.new_label = argv[optind+2];
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

	dump_newick(tree->root);
}

struct hash *set_map(struct parameters params)
{
	if (NULL != params.map_filename)
		return read_map(params.map_filename);

	struct hash *map = create_hash(1);
	if (NULL == map) { perror(NULL); exit(EXIT_FAILURE); }

	char *val = strdup(params.new_label);
	if (! hash_set(map, params.old_label, val)) {
		perror(NULL); exit(EXIT_FAILURE);
	}

	return map;
}

int main(int argc, char *argv[])
{
	struct rooted_tree *tree;	
	struct hash *rename_map;
	struct parameters params;
	
	params = get_params(argc, argv);

	rename_map = set_map(params);

	while (NULL != (tree = parse_tree())) {
		process_tree(tree, rename_map, params);
		destroy_all_rnodes(NULL);
		destroy_tree(tree);
	}

	struct llist *keys = hash_keys(rename_map);
	if (NULL == keys) { perror(NULL); exit(EXIT_FAILURE); }
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
