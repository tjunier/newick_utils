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
/* generate.c - tree generator */

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <assert.h>

#include "tree_models.h"

enum tree_models { GEOMETRIC_TREE, TIME_LIMITED_TREE };

const double	DEFAULT_MEAN_BRANCH_LENGTH = 1.0;
const double	DEFAULT_DURATION = 3;

struct parameters {
	enum tree_models tree_model;
	int seed;
	double duration;
	double mean_branch_length;
	double prob_node_has_children;
};

void help(char *argv[])
{
	printf (
"Generates a random tree\n"
"\n"
"Synopsis\n"
"--------\n"
"\n"
"%s [-dghlps]\n"
"\n"
"Input\n"
"-----\n"
"\n"
"None\n"
"\n"
"Output\n"
"------\n"
"\n"
"The generated tree. By default, this is a \"time-limited\" tree: branches\n"
"are \"grown\" using exponentially distributed lengths. If the length\n"
"exceeds the time limit, growth stops; otherwise the lineage splits in two,\n"
"and the process is repeated on the children.\n"
"\n"
"Options\n"
"-------\n"
"\n"
"    -d <float>: sets the maximum depth (time limit). Default: 3.0\n"
"       Only for time-limited trees.\n"
"    -g: generate a geometric tree instead of a time-limited one.\n"
"        Each node has a fixed probability of having 2 children.\n"
"    -h: print this message and exit\n"
"    -l <float>: sets the average branch length (default: 1.0)\n"
"       Only for time-limited trees.\n"
"    -p <float>: sets the probability of a node having (2) children.\n"
"       Only for geometric trees. WARNING: if > 0.5, the tree will\n"
"       probably grow 'forever'. On the other hand, low values will\n"
"       result in most trees having only a root, as the probability\n"
"       that the root has no children is 1-p, like for every other node.\n"
"    -s <int>: sets the pseudorandom number generator's seed\n"
"       (default: 0.1)\n"
"\n"
"Examples\n"
"--------\n"
"\n"
"# Generate a random time-limited tree\n"
"\n"
"$ %s\n",
	argv[0],
	argv[0]
	);
}

struct parameters get_params(int argc, char *argv[])
{

	struct parameters params;
	params.tree_model = TIME_LIMITED_TREE;
	params.seed = time(NULL);
	params.duration = DEFAULT_DURATION;
	params.mean_branch_length = DEFAULT_MEAN_BRANCH_LENGTH;
	params.prob_node_has_children = 0.1;

	int opt_char;
	while ((opt_char = getopt(argc, argv, "d:ghl:p:s:")) != -1) {
		switch (opt_char) {
		case 'd':
			params.duration = atof(optarg);	
			break;
		case 'g':
			params.tree_model = GEOMETRIC_TREE;
			break;
		case 'h':
			help(argv);
			exit(EXIT_SUCCESS);
		case 'l':
			params.mean_branch_length = atof(optarg);	
			break;
		case 'p':
			params.prob_node_has_children = atof(optarg);
			break;
		case 's':
			params.seed = atoi(optarg);
			break;
		}
	}

	/* check arguments */
	if ((argc - optind) != 0)	{
		fprintf(stderr, "Usage: %s [-hs]\n",
				argv[0]);
		exit(EXIT_FAILURE);
	}

	return params;
}

int main(int argc, char *argv[])
{
	struct parameters params = get_params(argc, argv);
	srand(params.seed);

	int result;
	switch (params.tree_model) {
	case GEOMETRIC_TREE:
		result = geometric_tree(params.prob_node_has_children);
		break;
	case TIME_LIMITED_TREE:
		result = time_limited_tree(1.0 / params.mean_branch_length,
			params.duration);
		break;
	default:
		assert(0);	/* programmer error */
	}
	if (! result) { perror(NULL); exit(EXIT_FAILURE); }

	exit(EXIT_SUCCESS);
}
