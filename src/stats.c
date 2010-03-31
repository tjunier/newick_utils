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
/* nw_stats - prints statistics and properties of trees */

#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include <string.h>

#include "parser.h"
#include "list.h"
#include "tree.h"
#include "rnode.h"
//#include "masprintf.h"

struct parameters {

};

struct tree_properties {
	enum tree_type type;
	int num_nodes;
	int num_leaves;
	int num_dichotomies;
};

static void help(char *argv[])
{
	printf (
"Prints statistics about trees\n"
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
"which case trees are read from standard input).\n"
"\n"
"Output\n"
"------\n"
"\n"
"Prints information about the trees in the input, one line per tree.\n"
"By default, each line contains the following fields: type, number of\n"
"leaves [TODO: add rest here]\n"
"\n"
"Options\n"
"-------\n"
"\n"
"    -h: print this message and exit\n"
"\n"
"Examples\n"
"--------\n"
"\n"
"# default statistics:\n"
"\n"
"$ %s data/catarrhini.nw\n",
	argv[0],
	argv[0]
		);
}

static void get_params(int argc, char *argv[])
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
		fprintf(stderr, "Usage: %s [-h] <filename|->\n", argv[0]);
		exit(EXIT_FAILURE);
	}
}

static int get_num_dichotomies(struct rooted_tree *tree)
{
	int dichotomies = 0;
	struct list_elem *el;
	for (el = tree->nodes_in_order->head; NULL != el; el =el->next) {
		struct rnode *current = (struct rnode *) el->data;
		int num_kids = current->children->count;
		if (2 == num_kids)
			dichotomies++;
	}
	return dichotomies;
}

static char *type_string(enum tree_type type)
{
	switch(type) {
	case TREE_TYPE_CLADOGRAM:
		return "Cladogram";
	case TREE_TYPE_PHYLOGRAM:
		return "Phylogram";
	case TREE_TYPE_NEITHER:
		return "Neither";
	case TREE_TYPE_UNKNOWN:
		return "Unknown";	/* weird! should not happen. */
	}
	return NULL; /* dummy, won't compile with -Wall otherwise */
}

static void write_info (struct tree_properties *props)
{
	/* for now there is only one output format, but we could add more */
	// later: switch on the format, or pass output function ptr */
	printf("Type\t#nodes\t#leaves\t#dichot\n");
	printf("%s\t%d\t%d\t%d\n",
			type_string(props->type),
			props->num_nodes,
			props->num_leaves,
			props->num_dichotomies);
}

static void process_tree(struct rooted_tree *tree)
{
	struct tree_properties props;

	props.type = get_tree_type(tree);
	props.num_nodes = tree->nodes_in_order->count;
	props.num_leaves = leaf_count(tree);
	props.num_dichotomies = get_num_dichotomies(tree);

	write_info(&props);
}

int main (int argc, char* argv[])
{
	struct rooted_tree *tree;

	get_params(argc, argv);

	while ((tree = parse_tree()) != NULL) {
		process_tree(tree);
		destroy_tree(tree, FREE_NODE_DATA);
	}

	return 0;
}
