/* generate.c - tree generator */

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>

#include "tree_models.h"

const double	DEFAULT_MEAN_BRANCH_LENGTH = 1.0;
const double	DEFAULT_DURATION = 3;

struct parameters {
	int seed;
	double duration;
	double mean_branch_length;
};

void help(char *argv[])
{
	printf (
"Generates a random tree\n"
"\n"
"Synopsis\n"
"--------\n"
"\n"
"%s [-cdhsx]\n"
"\n"
"Input\n"
"-----\n"
"\n"
"None\n"
"\n"
"Output\n"
"------\n"
"\n"
"The generated tree. Branches are \"grown\" using exponentially distributed\n"
"lengths. If the length exceeds the time limit, growth stops; otherwise the\n"
"lineage splits in two, and the process is repeated on the children.\n"
"\n"
"Options\n"
"-------\n"
"\n"
"    -d <float>: sets the maximum depth (time limit). Default: 3.0\n"
"    -h: print this message and exit\n"
"    -l <float>: sets the average branch length (default: 1.0)\n"
"    -s  <int>: sets the pseudorandom number generator's seed\n"
"    0.1)\n"
"\n"
"Examples\n"
"--------\n"
"\n"
"# Generate a random tree\n"
"\n"
"$ %s\n",
	argv[0],
	argv[0]
	);
}

struct parameters get_params(int argc, char *argv[])
{

	struct parameters params;
	params.seed = time(NULL);
	params.duration = DEFAULT_DURATION;
	params.mean_branch_length = DEFAULT_MEAN_BRANCH_LENGTH;

	int opt_char;
	while ((opt_char = getopt(argc, argv, "d:hl:s:")) != -1) {
		switch (opt_char) {
		case 'd':
			params.duration = atof(optarg);	
			break;
		case 'h':
			help(argv);
			exit(EXIT_SUCCESS);
		case 'l':
			params.mean_branch_length = atof(optarg);	
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

	// geometric_tree(params.prob_node_has_children);

	// The 1st parameter is the exponential distribution's rate parameter,
	// which is the inverse of the mean.
	time_limited_tree(1.0 / params.mean_branch_length,
			params.duration);
	
	exit(EXIT_SUCCESS);
}
