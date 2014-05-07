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
/* condense: simplifies tree */
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>

#include "tree.h"
#include "rnode.h"
#include "parser.h"
#include "to_newick.h"
#include "readline.h"
#include "hash.h"
#include "list.h"
#include "link.h"
#include "masprintf.h"

enum actions { PURE_CLADES, STAIR_NODES }; /* not sure we'll keep stair nodes */

struct group_data {
	char *name;
	char *repr_member;	/* the label of a "representative" for the group */
	int size;
};

struct parameters {
	enum actions action;	/* for now, only condense pure clades */
	char *grp_map_fname;
};

void help(char *argv[])
{
	printf(
"Simplifies a tree by collapsing certain nodes.\n"
"\n"
"Synopsis\n"
"--------\n"
"\n"
"%s [-hm:] <tree|->\n"
"\n"
"Input\n"
"-----\n"
"\n"
"Argument is either the name of a file that contains one or more trees, or '-',\n"
"in which case the trees are read on stdin.\n"
"\n"
"Output\n"
"------\n"
"\n"
"The tree(s) with pure clades collapsed. A pure clade is a clade in which\n"
"all leaves have the same label, and it is replaced by a leaf of the same\n"
"label: (A,(B,(B,B))); has a pure clade of B, and will be replaced by\n"
"(A,B);. The collapsed clade's support value (if any) is preserved, as is\n"
"its parent edge's length (if specified).\n"
"\n"
"Options\n"
"-------\n"
"\n"
"   -h: prints this message and exits\n"
"   -m <map file>: uses a group map. This is a text file that lists one\n"
"      label and one group name per line. For example a file with the\n"
"      following contents\n"
"            Homo Africa\n"
"            Hylobates Asia\n"
"            Gorilla Africa\n"
"            Pan Africa\n"
"            Pongo Asia\n"
"      maps the generic names of some apes to their continent of origin.\n"
"      That is, it defines the groups 'Asia' and 'Africa'. Labels and group\n"
"      names are white-separated and should not contain spaces.\n"
"        Clades consisting entirely of leaves belonging to a single group\n"
"      will be replaced by a single leaf whose label has the following\n"
"      structure: <group name>_<sample>_<size>, where <sample> is the label\n"
"      of one of the members of the clade, and <size> is the number of\n"
"      members of the clade.\n"
"        Applied to the Old world primates tree 'data/catarrhini', the above\n"
"      map would condense all African apes into a single leaf (since they\n"
"      form a clade) with label 'Africa_Homo_3'. It would not be able to\n"
"      condense further, however, because Pongo belong to group 'Asia'.\n"
"\n"
"Example\n"
"-------\n"
"\n"
"# produce a tree of families from a genus tree in which all genus names\n"
"# have been replaced by family names (see nw_rename) - look at\n"
"# data/falconiformes\n"
"$ %s data/falc_families\n"
"\n"
"# condense by geographic origin\n"
"$ %s -m data/catarrhini_geog.map data/catarrhini\n",
argv[0],
argv[0],
argv[0]
	);
}

struct parameters get_params(int argc, char *argv[])
{

struct parameters params;

params.action = PURE_CLADES;
params.grp_map_fname = NULL;

/* parse options and switches */
int opt_char;
while ((opt_char = getopt(argc, argv, "hm:s")) != -1) {
	switch (opt_char) {
	case 'h':
		help(argv);
		exit(EXIT_SUCCESS);
	case 'm':
		// TODO: check return values of strdup() (in ALL the code)!
			params.grp_map_fname = optarg;
			break;
		case 's':
			/* Not implemented yet - not sure if it will be */
			params.action = STAIR_NODES;
			break;
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
		fprintf(stderr, "Usage: %s [-hm:] <filename|->\n",
				argv[0]);
		exit(EXIT_FAILURE);
	}

	return params;
}

// TODO: this f() has been duplicated from condense.c. It should be removed from
// there and from here, and moved to hash.c

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

/* Like all_children_have_same_label() in rnode.c, but returns true if all
 * children belong to the same group, as determined by the rnode->data member.
 * Sets group to the common group, if any, or else to NULL.  */

bool all_children_in_same_group(struct rnode *node, struct group_data *grp_data)
{

	if (is_leaf(node))
		return false;

	/* get first child's group */
	struct rnode *curr = node->first_child;
	struct group_data *g_data = curr->data;
	char *ref_group = g_data->name;

	/* iterate over other children, and compare their group to the first's
	 * */

	int status = 1;
	int size = 1;
	for (curr = curr->next_sibling; curr; curr = curr->next_sibling) {
		g_data = curr->data;
		char *curr_group = g_data->name;
		if (0 != strcmp(ref_group, curr_group))
			status *= 0; /* found a different group */
		size += g_data->size;
	}

	grp_data->name = strdup(g_data->name);
	grp_data->repr_member = strdup(g_data->repr_member);
	grp_data->size = size;

	return (bool) status;
}

/* Same as collapse_pure_clades() (tree.c), but collapses clades of the same
 * group, usig a label->group map. */

void collapse_by_groups(struct rooted_tree *tree, struct hash *group_map)
{
	struct list_elem *el;		
	struct group_data *grp_data;

	for (el = tree->nodes_in_order->head; NULL != el; el = el->next) {
		struct rnode *current = el->data;
		grp_data = malloc(sizeof(grp_data));
		if (NULL == grp_data) { perror(NULL); exit (EXIT_FAILURE); }
		current->data = grp_data;

		if (is_leaf(current)) {
			char *group = hash_get(group_map, current->label);
			if (NULL == group) 
				grp_data->name = strdup("");
			else
				grp_data->name = strdup(group);
			grp_data->repr_member = strdup(current->label);
			grp_data->size = 1;
			
			continue;
		}

		grp_data->name = strdup("MIXED");
		grp_data->repr_member = strdup("");
		grp_data->size = -1;

		/* attempt collapse only if all children are leaves (any pure
		 * subtree will have been collapsed to a leaf by now) */
		if (! all_children_are_leaves(current)) continue;

		/* this also sets grp_data */
		if (all_children_in_same_group(current, grp_data)) {
			/*
			 fprintf(stderr,
				"%s and %d other descendants of '%s' "
				"belong to group '%s'\n",
				grp_data->repr_member,
				grp_data->size - 1,
				current->label,
				grp_data->name);
				*/
			remove_children(current);
			char *new_label = masprintf("%s_%s_%d",
					grp_data->name, grp_data->repr_member,
					grp_data->size);
			free(current->label);
			current->label = new_label;
		}
	}
}


int main(int argc, char *argv[])
{
	struct rooted_tree *tree;	
	struct parameters params;
	struct hash *group_map = NULL;
	
	params = get_params(argc, argv);
	if (NULL != params.grp_map_fname)
		group_map = read_map(params.grp_map_fname);

	// debug
	// if (NULL != group_map) dump_hash(group_map, NULL);

	while (true) {
		tree = parse_tree();
		if (NULL != tree) {
			if (NULL == group_map)
				collapse_pure_clades(tree);
			else
				collapse_by_groups(tree, group_map);

			dump_newick(tree->root);
			destroy_all_rnodes(NULL);
			destroy_tree(tree);
		}
		else switch (newick_parser_status) {
			case PARSER_STATUS_EMPTY:	/* EOF, etc. */
				/* goto is ok to break "twice" */
				goto end;
			case PARSER_STATUS_PARSE_ERROR:
				/* for now, the parser prints the error message
				 * */
				break;
			case PARSER_STATUS_MALLOC_ERROR:
				perror(NULL);
				exit(EXIT_FAILURE);
			default:
				assert(0);	/* programmer error */
		}
	}

end:

	return 0;
}
