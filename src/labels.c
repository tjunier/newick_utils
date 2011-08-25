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
/* nw_labels:  print out the labels of trees */

#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include <string.h>
#include <stdbool.h>

#include "parser.h"
#include "tree.h"
#include "rnode.h"
#include "list.h"
#include "common.h"

struct parameters {
	bool show_inner_labels;
	bool show_leaf_labels;
	bool show_only_root_label;
	char separator;
};

void help(char *argv[])
{
	printf (
"Extracts the tree's labels\n"
"\n"
"Synopsis\n"
"--------\n"
"\n"
"%s [-hILrt] <newick trees filename|->\n"
"\n"
"Input\n"
"-----\n"
"\n"
"Argument is the name of a file that contains Newick trees, or '-' (in\n"
"which case trees are read from standard input).\n"
"\n"
"Output\n"
"------\n"
"\n"
"By default, prints all labels that occur in the tree, in the same order as\n"
"in the Newick, one per line. Empty labels produce no output.\n"
"\n"
"Options\n"
"-------\n"
"\n"
"    -h: print this message and exit\n"
"    -I: don't print labels of inner nodes\n"
"    -L: don't print leaf labels\n"
"    -r: print only the root's label\n"
"    -t: TAB-separated - print on a single line, separated by tab stops.\n"
"\n"
"Examples\n"
"--------\n"
"\n"
"# just show labels\n"
"$ %s data/catarrhini\n"
"\n"
"# count leaves\n"
"$ %s -I data/catarrhini | wc -l\n",
	argv[0],
	argv[0],
	argv[0]
	);
}

struct parameters get_params(int argc, char *argv[])
{

	struct parameters params;

	/* defaults */
	params.show_inner_labels = true;
	params.show_leaf_labels = true;
	params.show_only_root_label = false;
	params.separator = '\n';

	int opt_char;
	while ((opt_char = getopt(argc, argv, "hILrt")) != -1) {
		switch (opt_char) {
		case 'h':
			help(argv);
			exit(EXIT_SUCCESS);
		case 'I':
			params.show_inner_labels = false;
			break;
		case 'L':
			params.show_leaf_labels = false;
			break;
		case 'r':
			params.show_only_root_label = true;
			break;
		case 't':
			params.separator = '\t';
			break;
		default:
			fprintf (stderr, "Unknown option '-%c'\n", opt_char);
			exit (EXIT_FAILURE);
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
		fprintf(stderr, "Usage: %s [-hILt] <filename|->\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	return params;
}


void process_tree(struct rooted_tree *tree, struct parameters params)
{

	struct list_elem *elem;
	int first_line = 1;

	if (params.show_only_root_label) {
		printf ("%s\n", tree->root->label);
		return;
	}

	for (elem = tree->nodes_in_order->head; NULL != elem; elem = elem->next) {
		struct rnode *current = (struct rnode *) elem->data;
		char *label = current->label;

		if (strcmp("", label) == 0)
			continue;

		if (is_leaf(current)) {
			if (params.show_leaf_labels) {
				if (! first_line) putchar(params.separator);
				printf ("%s", label);
				if (first_line) first_line = 0;
			}
		} else {
			if (params.show_inner_labels) {
				if (! first_line) putchar(params.separator);
				printf ("%s", label);
				if (first_line) first_line = 0;
			}
		}
		
	}

	putchar('\n');
}

int main (int argc, char* argv[])
{
	struct rooted_tree *tree;
	struct parameters params;

	params = get_params(argc, argv);

	while ((tree = parse_tree()) != NULL) {
		process_tree(tree, params);
		destroy_tree(tree);
		destroy_all_rnodes(NULL);
	}

	return 0;
}
