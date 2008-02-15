#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include "enode.h"
#include "rnode.h"
#include "link.h"
#include "address_parser.h"

extern int adsdebug;

struct enode *expression_root;

struct parameters {
	char * address;
};

struct parameters get_params(int argc, char *argv[])
{
	struct parameters params;

	/* check arguments */
	if (3 == (argc - optind))	{
		if (0 != strcmp("-", argv[optind])) {
			FILE *fin = fopen(argv[optind], "r");
			extern FILE *nwsin;
			if (NULL == fin) {
				perror(NULL);
				exit(EXIT_FAILURE);
			}
			nwsin = fin;
		}
		params.address = argv[optind+1];
	} else {
		fprintf(stderr, "Usage: %s <filename|-> <addr> <act>\n",
				argv[0]);
		exit(EXIT_FAILURE);
	}

	return params;
}

int main(int argc, char* argv[])
{
	struct rnode *nodeA = create_rnode("A");
	struct rnode *nodeB = create_rnode("B");
	struct rnode *nodeC = create_rnode("C");
	struct rnode *noded = create_rnode("d");
	struct rnode *nodee = create_rnode("e");

	link_p2c(noded, nodeB, "1");
	link_p2c(noded, nodeC, "1");
	link_p2c(nodee, nodeA, "1");
	link_p2c(nodee, noded, "1");

	struct parameters params = get_params(argc, argv);

	adsdebug = 0;
	address_scanner_set_input(params.address);
	adsparse(); /* sets 'expression_root' */
	address_scanner_clear_input();

	enode_eval_set_current_rnode(nodeA);
	printf ("%g\n", eval_enode(expression_root));

	return 0;
}
