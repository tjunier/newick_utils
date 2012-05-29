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
/* trim.c: trim tree at certain depth (in distance or number of ancestors) */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>

#include "to_newick.h"
#include "tree.h"
#include "parser.h"
#include "masprintf.h"
#include "rnode.h"
#include "list.h"
#include "link.h"

enum {DEPTH_DISTANCE, DEPTH_ANCESTORS};

struct node_data {
	bool trimmed;
	int ancestry_depth;
	double distance_depth;
};

struct parameters {
	int depth_type;
	double threshold;
};

void help(char *argv[])
{
	printf (
"Trims a tree at a certain depth.\n"
"\n"
"Synopsis\n"
"--------\n"
"\n"
"%s [-ah] <newick trees filename|-> <maximum depth>\n"
"\n"
"Input\n"
"-----\n"
"\n"
"The first argument is the name of a file that contains Newick trees, or '-'\n"
"(in which case trees are read from standard input). The second argument is\n"
"the maximum depth: nodes deeper than this will be trimmed.\n"
"\n"
"Output\n"
"------\n"
"\n"
"A tree whose depth is at most the maximum depth (second argument).\n"
"Effectively, it is like cutting the tree at that value: nodes that are too\n"
"deep get trimmed, and internal nodes also lose their children.\n"
"\n"
"Options\n"
"-------\n"
"\n"
"    -a: the maximum depth is expressed in number of ancestors, not distance.\n"
"        Nodes are not shortened, but no node is retained that has more\n"
"        ancestors than the maximum.\n"
"    -h: print this message and exit\n"
"\n"
"Examples\n"
"--------\n"
"\n"
"# Cut tree at depth 20\n"
"$ %s data/catarrhini 20\n"
"\n"
"# Discard nodes with more than 3 ancestors\n"
"$ %s -a data/catarrhini 3\n",
	argv[0],
	argv[0],
	argv[0]
	);
}

struct parameters get_params(int argc, char *argv[])
{
	struct parameters params;
	params.depth_type = DEPTH_DISTANCE;

	int opt_char;
	while ((opt_char = getopt(argc, argv, "ah")) != -1) {
		switch (opt_char) {
		case 'a':
			params.depth_type = DEPTH_ANCESTORS;
			break;
		case 'h':
			help(argv);
			exit (EXIT_SUCCESS);
		default:
			fprintf (stderr, "Unknown option '-%c'\n", opt_char);
			exit (EXIT_FAILURE);
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
		optind++;	/* optind is now index of 2nd arg - depth */
		params.threshold = atof(argv[optind]);
		/* A value of n means "n ancestors or less" */
		if (DEPTH_ANCESTORS == params.depth_type)
			params.threshold -= 1;
	} else {
		fprintf(stderr, "Usage: %s [-ah] <filename|-> <depth>\n",
				argv[0]);
		exit(EXIT_FAILURE);
	}

	return params;
}

void trim(struct rnode *node, struct parameters params)
{
	struct node_data *ndata = node->data;

	if (DEPTH_DISTANCE == params.depth_type) {
		/* Shrink parent edge length */
		double excess = ndata->distance_depth - params.threshold;
		double trimmed_edge_length = node->edge_length - excess;
		free(node->edge_length_as_string);
		char *new_length = masprintf("%g", trimmed_edge_length);
		if (NULL == new_length) { perror(NULL); exit(EXIT_FAILURE); }
		node->edge_length_as_string = new_length;
	}

	remove_children(node);	/* no effect on leaves */

	ndata->trimmed = true;
}

void process_tree(struct rooted_tree *tree, struct parameters params)
{
	struct llist *nodes_in_preorder;
	struct list_elem *elem;
	struct rnode *node;

	nodes_in_preorder = llist_reverse(tree->nodes_in_order);
	if (NULL == nodes_in_preorder) { perror(NULL); exit(EXIT_FAILURE); }
	node = (struct rnode *) nodes_in_preorder->head->data; /* root */
	struct node_data * ndata = malloc(sizeof(struct node_data));
	if (NULL == ndata) { perror(NULL); exit(EXIT_FAILURE); }
	ndata->distance_depth = 0.0;
	ndata->ancestry_depth = 0;
	ndata->trimmed = false;
	node->data = ndata;

	/* This starts just AFTER the root! */
	for (elem = nodes_in_preorder->head->next;
			NULL != elem; elem = elem->next) {
		node = (struct rnode *) elem->data;
		struct node_data *parent_data = node->parent->data;
		/* allocate this node's data structure */
		ndata = malloc(sizeof(struct node_data));
		if (NULL == ndata) { perror(NULL); exit(EXIT_FAILURE); }
		node->data = ndata;

		/* See if parent has been trimmed */
		if (parent_data->trimmed) {
			// printf ("trimmed.\n");
			ndata->trimmed = true; 	/* inherit trimmed status */
			continue;
		} else {
			/* If we don't do this, ndata->trimmed may
			 * contain garbage!  (thanks Valgrind :-) */
			ndata->trimmed = false;
		}


		/* Parent not trimmed: See if we must trim this node. */
	
		/* compute this node's depth measures */
		double edge_length = atof(node->edge_length_as_string);
		node->edge_length = edge_length;
		ndata->distance_depth = edge_length + parent_data->distance_depth;
		ndata->ancestry_depth = 1 + parent_data->ancestry_depth;

		switch (params.depth_type) {
		case DEPTH_DISTANCE:
			if (ndata->distance_depth > params.threshold)
				trim(node, params);
			break;
		case DEPTH_ANCESTORS:
			if (ndata->ancestry_depth > params.threshold)
				trim(node, params);
			break;
		default:
			assert (false);	/* programmer error */
			exit(EXIT_FAILURE);
		}
	}

	destroy_llist(nodes_in_preorder);

}

int main(int argc, char *argv[])
{
	struct rooted_tree *tree;	
	struct parameters params;
	
	params = get_params(argc, argv);

	while (NULL != (tree = parse_tree())) {
		process_tree(tree, params);
		dump_newick(tree->root);
		destroy_all_rnodes(NULL);
		destroy_tree(tree);
	}

	return 0;
}
