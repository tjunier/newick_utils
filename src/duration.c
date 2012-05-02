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
/* nw_duration - read a tree with node ages (absolute) and recode it as lengths (durations) */

#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include <string.h>

#include "parser.h"
#include "to_newick.h"
#include "list.h"
#include "tree.h"
#include "rnode.h"
#include "masprintf.h"

void help(char *argv[])
{
	printf (
"Convert node ages to durations\n"
"\n"
"Synopsis\n"
"--------\n"
"\n"
"%s [-h] <newick trees filename|->\n"
"\n"
"Input\n"
"-----\n"
"\n"
"Argument is the name of a file that contains Newick trees, or '-' (in\n"
"which case trees are read from standard input). The branch \"lengths\"\n"
"are interpreted as (absolute) dates, e.g. in Mya.\n"
"\n"
"Output\n"
"------\n"
"\n"
"Prints the input trees with branch lengths representing elapsed time.\n"
"This is handy if you have data about ages rather than durations, e.g.\n"
"you will typically know that two sister clades split 450 Mya, or that\n"
"some clade lasted from the early Devonian to the present. By using the\n"
"Newick format's 'length' slot to code ages, you can use this program\n"
"to generate a tree with correct branch lengths.\n"
"\n"
"Options\n"
"-------\n"
"\n"
"    -h: print this message and exit\n"
"\n"
"Examples\n"
"--------\n"
"\n"
"# File data/vertebrates.nw is age-encoded (every node's \"length\" is\n"
"# actually the age (in million years ago) when the branch split into\n"
"# the daughter branches). To get a usual, duration-encoded tree:\n"
"\n"
"$ %s data/vertebrates.nw\n"
"\n"
"# This works well with options -Ir, -t and -u to nw_display:\n"
"\n"
"$ %s data/vertebrates.nw | nw_display -Ir -t -u 'million years ago' -\n",
	argv[0],
	argv[0],
	argv[0]
		);
}

void get_params(int argc, char *argv[])
{


	int opt_char;
	while ((opt_char = getopt(argc, argv, "h")) != -1) {
		switch (opt_char) {
		case 'h':
			help(argv);
			exit(EXIT_SUCCESS);
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
		fprintf(stderr, "Usage: %s [-bhIL] <filename|->\n", argv[0]);
		exit(EXIT_FAILURE);
	}
}

void process_tree(struct rooted_tree *tree)
{
	struct list_elem *el;
	
	for (el = tree->nodes_in_order->head; NULL != el; el =el->next) {
		struct rnode *current = (struct rnode *) el->data;
		if (is_root(current)) {
			/* set to none */
			free(current->edge_length_as_string);
			current->edge_length_as_string = strdup("");
		}
		else {
			double age = -1.0;
			if (strcmp("", current->edge_length_as_string) == 0)
				age = 0.0;
			else 
				age = atof(current->edge_length_as_string);	

			/* Note: we are assuming that only leaves can have an
			 * empty length_as_string. Thus we do not check
			 * for emptiness before calling atof(). */
			double parent_age = atof(current->parent->
				edge_length_as_string);
			double edge_length = parent_age - age;
			free(current->edge_length_as_string);
			current->edge_length_as_string = masprintf("%g",
					edge_length);
		}
	}
}

int main (int argc, char* argv[])
{
	struct rooted_tree *tree;

	get_params(argc, argv);

	while ((tree = parse_tree()) != NULL) {
		process_tree(tree);
		dump_newick(tree->root);
		destroy_all_rnodes(NULL);
		destroy_tree(tree);
	}

	return 0;
}
