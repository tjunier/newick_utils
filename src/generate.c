/* generate.c - tree generator */

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>

#include "tree_models.h"

#define DEFAULT_PROB_HAS_CHILDREN 0.4
#define DEFAULT_DURATION 3

struct parameters {
	int seed;
	double prob_node_has_children;
	double duration;
	double prob_extinction;
};

void help(char *argv[])
{
	printf (
"Generates a random tree\n"
"\n"
"Synopsis\n"
"--------\n"
"\n"
"%s [-hs]\n"
"\n"
"Input\n"
"-----\n"
"\n"
"None\n"
"\n"
"Output\n"
"------\n"
"\n"
"The generated tree.\n"
"\n"
"Options\n"
"-------\n"
"\n"
"    -c <float> sets the probability that a node has children\n"
"    -h: print this message and exit\n"
"    -s  <int>: sets the pseudorandom number generator's seed\n"
"\n"
"Examples\n"
"--------\n"
"\n"
"# Generate a random tree\n"
"\n"
"%s\n",
	argv[0],
	argv[0]
	);
}

struct parameters get_params(int argc, char *argv[])
{

	struct parameters params;
	params.seed = time(NULL);
	params.prob_node_has_children = DEFAULT_PROB_HAS_CHILDREN;
	params.duration = DEFAULT_DURATION;

	int opt_char;
	while ((opt_char = getopt(argc, argv, "c:d:hs:x:")) != -1) {
		switch (opt_char) {
		case 'c':
			params.prob_node_has_children = atof(optarg);	
			break;
		case 'd':
			params.duration = atof(optarg);	
			break;
		case 'h':
			help(argv);
			exit(EXIT_SUCCESS);
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
	time_limited_tree(params.prob_extinction+params.prob_node_has_children, params.duration);
	
	exit(EXIT_SUCCESS);
}
