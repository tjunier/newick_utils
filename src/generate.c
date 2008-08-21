/* generate.c - tree generator */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

#include "rnode.h"
#include "list.h"
#include "common.h"
#include "link.h"
#include "to_newick.h"

static const double prob_node_has_children = 0.4;

struct parameters {
	int seed;
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

	int opt_char;
	while ((opt_char = getopt(argc, argv, "hs:")) != -1) {
		switch (opt_char) {
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

int gets_children()
{
	double rn = (double) rand() / RAND_MAX;

	if (rn <= prob_node_has_children)
		return TRUE;
	else
		return FALSE;
}

/* Visits a leaf: probabilistically adds children to the leaf, and adds those
 * children to the leaves queue (since they are new leaves) */

void visit_leaf(struct rnode *leaf, struct llist *leaves_queue)
{
	// printf ("visiting leaf %p (%s)\n", leaf, leaf->label);
	if (gets_children()) {
		// printf (" gets children\n");
		struct rnode *kid1 = create_rnode("kid1");	
		struct rnode *kid2 = create_rnode("kid2");	
		link_p2c(leaf, kid1, "1");
		link_p2c(leaf, kid2, "1");
		append_element(leaves_queue, kid1);
		append_element(leaves_queue, kid2);
	} else {
		// printf (" gets no children\n");
	}
}

int main(int argc, char *argv[])
{
	struct parameters params = get_params(argc, argv);

	struct llist *leaves_queue = create_llist();
	struct rnode *root = create_rnode("root");

	srand(params.seed);

	append_element(leaves_queue, root);

	/* The queue contains any newly added leaves. We visit them in turn,
	 * possibly adding new leaves to th equeue. The process stops when no
	 * new leaves have been added. */

	while (leaves_queue->count > 0) {
		int nb_leaves_to_visit = leaves_queue->count;
		/* Iterate over leaves. Note that new leaves can be added at
		 * the end of the queue */
		for (; nb_leaves_to_visit > 0; nb_leaves_to_visit--) {
			struct rnode *current_leaf = shift(leaves_queue);
			visit_leaf(current_leaf, leaves_queue);
		}
	}

	char *newick = to_newick(root);
	printf("%s\n", newick);
	free(newick);

	exit(EXIT_SUCCESS);
}
