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

enum actions { PURE_CLADES, STAIR_NODES }; /* not sure we'll keep stair nodes */

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
"   -m <map file>: uses a group map [TODO: complete description]\n"
"\n"
"Example\n"
"-------\n"
"\n"
"# produce a tree of families from a genus tree in which all genus names\n"
"# have been replaced by family names (see nw_rename) - look at\n"
"# data/falconiformes\n"
"$ %s data/falc_families\n",
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

int main(int argc, char *argv[])
{
	struct rooted_tree *tree;	
	struct parameters params;
	struct hash *group_map = NULL;
	
	params = get_params(argc, argv);
	if (NULL != params.grp_map_fname)
		group_map = read_map(params.grp_map_fname);

	// debug
	if (NULL != group_map) dump_hash(group_map, NULL);

	while (true) {
		tree = parse_tree();
		if (NULL != tree) {
			collapse_pure_clades(tree);
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
