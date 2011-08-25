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
#include <stdbool.h>

#include "parser.h"
#include "list.h"
#include "tree.h"
#include "rnode.h"
#include "common.h"

enum stats_output_format {STATS_OUTPUT_LINE, STATS_OUTPUT_COLUMN};

struct tree_properties {
	enum tree_type type;
	int num_nodes;
	int num_leaves;
	int num_dichotomies;
	int num_leaf_labels;
	int num_inner_labels;
};

struct parameters {
	enum stats_output_format output_format;
	void (* output_function)(struct tree_properties *);
	bool headers;
};

static void help(char *argv[])
{
	printf (
"Prints statistics about trees\n"
"\n"
"Synopsis\n"
"--------\n"
"\n"
"%s [-hHf:] <newick trees filename|->\n"
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
"Prints information about the trees in the input. The info is printed:\n"
"Tree type, number of nodes, number of leaves, number of dichotomies,\n"
"number of leaf labels, number of inner labels. By default, the fields\n"
"are printed on a line, like this:\n"
"Type:	Phylogram\n"
"#nodes:	19\n"
"#leaves:	10\n"
"#dichotomies:	9\n"
"#leaf labels:	10\n"
"#inner labels:	6\n"
"\n"
"Options\n"
"-------\n"
"\n"
"    -h: print this message and exit\n"
"    -f [lc]: format in lines (l) or columns (c). Default is l.\n"
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

static void print_line(struct tree_properties *props)
{
	printf("%s\t%d\t%d\t%d\t%d\t%d\n",
		type_string(props->type),
		props->num_nodes,
		props->num_leaves,
		props->num_dichotomies,
		props->num_leaf_labels,
		props->num_inner_labels
		);
}

static void print_column(struct tree_properties *props)
{
	printf("Type:\t%s\n#nodes:\t%d\n#leaves:\t%d\n#dichotomies:\t%d\n"
		"#leaf labels:\t%d\n#inner labels:\t%d\n",
		type_string(props->type),
		props->num_nodes,
		props->num_leaves,
		props->num_dichotomies,
		props->num_leaf_labels,
		props->num_inner_labels
		);
}

static struct parameters get_params(int argc, char *argv[])
{
	struct parameters params;

	params.output_function = print_column;
	params.output_format = STATS_OUTPUT_COLUMN;
	params.headers = false;

	int opt_char;
	while ((opt_char = getopt(argc, argv, "f:Hh")) != -1) {
		switch (opt_char) {
		case 'f':
			switch (optarg[0]) {
			case 'l':
			case 'L':
				params.output_format = STATS_OUTPUT_LINE;
				params.output_function = print_line;
				break;
			case 'c':
			case 'C':
				break;	/* keep defaults */
			default:
				fprintf (stderr,
				"WARNING: wrong argument to option -f\n");
			}
			break;
		case 'H':
			params.headers = false;
			break;
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
		fprintf(stderr, "Usage: %s [-fHh] <filename|->\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	return params;
}

/* Iterate once over all nodes, updating various statistics */

static int get_properties(struct rooted_tree *tree,
		struct tree_properties *props)
{
	props->num_dichotomies = 0;
	props->num_leaf_labels = 0;
	props->num_inner_labels = 0;

	struct list_elem *el;
	for (el = tree->nodes_in_order->head; NULL != el; el =el->next) {
		struct rnode *current = (struct rnode *) el->data;
		int num_kids = current->child_count;
		/* tests */
		if (2 == num_kids)
			props->num_dichotomies++;
		if (0 != strcmp("", current->label)) {
			if (is_leaf(current))
				props->num_leaf_labels++;
			if (is_inner_node(current))
				props->num_inner_labels++;
		}
	}
	return SUCCESS;
}

static void process_tree(struct rooted_tree *tree,
		void(* output_function)(struct tree_properties *))
{
	struct tree_properties props;

	props.type = get_tree_type(tree);
	props.num_nodes = tree->nodes_in_order->count;
	props.num_leaves = leaf_count(tree);
	if (! get_properties(tree, &props)) {
		perror("Could not get tree properties");
		exit(EXIT_FAILURE);
	}
	output_function(&props);
}

int main (int argc, char* argv[])
{

	struct parameters params = get_params(argc, argv);

	struct rooted_tree *tree;
	while ((tree = parse_tree()) != NULL) {
		process_tree(tree, params.output_function);
		destroy_all_rnodes(NULL);
		destroy_tree(tree);
	}

	return 0;
}
