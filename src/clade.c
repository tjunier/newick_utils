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
/* subtree: print subtree from a specified node */
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <regex.h>
#include <stdbool.h>

#include "tree.h"
#include "parser.h"
#include "nodemap.h"
#include "hash.h"
#include "to_newick.h"
#include "list.h"
#include "lca.h"
#include "rnode.h"
#include "common.h"
#include "link.h"
#include "subtree.h"

enum modes {EXACT, REGEXP};

struct parameters {
	struct llist *labels;
	bool check_monophyly;
	bool siblings;
	enum modes mode;
	char * regexp_string;
	regex_t *regexp;
	int context;	/* how many levels above LCA */
};

void help(char *argv[])
{
	printf(
"Extracts a subtree (clade) defined by labels.\n"
"\n"
"Synopsis\n"
"--------\n"
"\n"
"%s [-chmrs] <target tree filename|-> <label> [label]+\n"
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
"    -c <levels (int)>: give context, i.e. start the subtree not at the last\n"
"        common ancestor of the labels, but 'level' nodes higher (limited\n"
"        by the tree's root, of course).\n"
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
"$ %s -r data/HRV.nw '^POLIO.*'\n"
"\n"
"# clade defined by Homo and Pan, plus 1 level of context\n"
"$ %s -c 1 data/catarrhini Homo Pan\n",
	argv[0],
	argv[0],
	argv[0],
	argv[0],
	argv[0],
	argv[0],
	argv[0]
	      );
}

static regex_t * compile_regexp(char *regexp_string)
{
	int errcode;
	regex_t *preg = malloc(sizeof(regex_t));
	int cflags = 0;
	if (NULL == preg) {perror(NULL); exit(EXIT_FAILURE);}
	errcode = regcomp(preg, regexp_string, cflags);
	if (errcode) {
		size_t errbufsize = regerror(errcode, preg, NULL, 0);
		char *errbuf = malloc(errbufsize * sizeof(char));
		if (NULL == errbuf) {perror(NULL); exit(EXIT_FAILURE);}
		regerror(errcode, preg, errbuf, errbufsize);
		fprintf (stderr, "%s\n", errbuf);
		exit(EXIT_FAILURE);
	}

	return preg;
}

struct parameters get_params(int argc, char *argv[])
{

	struct parameters params;

	params.check_monophyly = false;
	params.siblings = false;
	params.mode = EXACT;
	params.context = 0;

	int opt_char;
	while ((opt_char = getopt(argc, argv, "c:hmrs")) != -1) {
		switch (opt_char) {
		case 'c':
			params.context = atoi(optarg);
			break;
		case 'h':
			help(argv);
			exit(EXIT_SUCCESS);
		case 'm':
			params.check_monophyly = true;
			break;
		case 'r':
			params.mode = REGEXP;
			break;
		case 's':
			params.siblings = true;
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
			if (! set_parser_input_filename(argv[optind])) {
				perror(NULL);
				exit(EXIT_FAILURE);
			}
		}
		struct llist *lbl_list;
		switch (params.mode) {
		case EXACT:
			lbl_list = create_llist();
			if (NULL == lbl_list) {
				perror(NULL); exit(EXIT_FAILURE);
			}
			optind++;	/* optind is now index of 1st label */
			for (; optind < argc; optind++) {
				if (! append_element(lbl_list, argv[optind])) {
					perror(NULL);
					exit(EXIT_FAILURE);
				}
			}
			params.labels = lbl_list;
			break;
		case REGEXP:
			optind++;	/* optind is now index of regexp */
			params.regexp_string = argv[optind];
			params.regexp = compile_regexp(params.regexp_string);
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

void process_tree(struct rooted_tree *tree, struct parameters params)
{
	struct llist *descendants;

	switch (params.mode) {
	case EXACT:
		descendants = nodes_from_labels(tree, params.labels);
		if (NULL == descendants) { perror(NULL); exit(EXIT_FAILURE); }
		if (0 == descendants->count) {
			fprintf (stderr, "WARNING: no label matches.\n");
			/* I don't consider this a failure: it is just the case
			 * that the tree does not contain the specified labels.
			 * */
			exit(EXIT_SUCCESS);
		}
		break;
	case REGEXP:
		descendants = nodes_from_regexp(tree, params.regexp);
		if (NULL == descendants) { perror(NULL); exit(EXIT_FAILURE); }
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

	struct rnode *subtree_root = lca_from_nodes(tree, descendants);
	if (NULL == subtree_root) { perror(NULL); exit(EXIT_FAILURE); }

	/* Jump up tree to get context, if any was required ('context' > 0) */
	int context;
	for (context = params.context; context > 0; context--)
		if (! is_root(subtree_root))
			subtree_root = subtree_root->parent;

	if (NULL != subtree_root) {
		if ((! params.check_monophyly) ||
		    (MONOPH_TRUE == is_monophyletic(descendants, subtree_root))) {
			/* monophyly of input labels is verified or not
			 * requested */
			if (params.siblings) {
				struct llist *sibs = siblings(subtree_root);
				if (NULL == sibs) {
					perror(NULL);
					exit(EXIT_FAILURE);
				}
				struct list_elem *el;
				for (el=sibs->head;NULL!=el;el=el->next) {
					dump_newick(el->data);
				}
				destroy_llist(sibs);
			} else {
				/* normal operation: print clade defined by
				 * labels. */
				dump_newick(subtree_root);
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
		destroy_all_rnodes(NULL);
		destroy_tree(tree);
	}

	if (EXACT == params.mode)
		destroy_llist(params.labels);
	else {
		/* This does not free 'params.regexp' itself, only memory
		 * pointed to by 'params.regexp' members and allocated by
		 * regcomp().*/
		regfree(params.regexp);
		/* Therefore: */
		free(params.regexp);
	}

	return 0;
}
