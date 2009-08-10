#include <stdio.h>
#include <complex.h> /* needed for cabs() */
#include <stdlib.h>

#include "rnode.h"
#include "list.h"
#include "link.h"
#include "tree_models.h"

#define TOLERANCE 0.000001

int test_reciprocal_exponential_CDF()
{
	const char *test_name = "test_reciprocal_exponential_CDF";

	double val = reciprocal_exponential_CDF(0,1);
	/* we use cabs() instead of abs() b/c it returns a double */
	if (cabs(val - 0) > TOLERANCE) {
		printf ("%s: expected 0, got %g\n", test_name, val);
		return 1;
	}
	val = reciprocal_exponential_CDF(0,10);
	if (cabs(val - 0) > TOLERANCE) {
		printf ("%s: expected 0, got %g\n", test_name, val);
		return 1;
	}
	val = reciprocal_exponential_CDF(0,.1);
	if (cabs(val - 0) > TOLERANCE) {
		printf ("%s: expected 0, got %g\n", test_name, val);
		return 1;
	}
	val = reciprocal_exponential_CDF(0.5,1);
	double exp = 0.693147; /* -ln(0.5) */
	if (cabs(val - exp) > TOLERANCE) {
		printf ("%s: expected %g, got %g (diff: %g)\n", test_name, exp, val, cabs(val-exp));
		return 1;
	}
	val = reciprocal_exponential_CDF(0.5,3);
	exp = 0.693147 / 3;
	if (cabs(val - exp) > TOLERANCE) {
		printf ("%s: expected %g, got %g (diff: %g)\n", test_name, exp, val, cabs(val-exp));
		return 1;
	}

	printf("%s ok.\n", test_name);
	return 0;
}

int test_tlt_grow_node()
{
	const char *test_name = "test_tlt_grow_node";
	double prior_remaining_time = 12.3; 	/* arbitrary units */
	struct rnode *parent = create_rnode("parent", "");
	struct rnode *kid = create_rnode("kid", "");
	kid->data = &prior_remaining_time;
	add_child(parent, kid);

	double posterior_remaining_time = tlt_grow_node(kid, 1.0, 0.5);
	double grown_length = strtod(kid->edge_length_as_string, NULL);

	double exp = prior_remaining_time - posterior_remaining_time;
	if (cabs(grown_length - exp) > TOLERANCE) {
		printf ("%s: expected %g, got %g\n", test_name, exp,
				grown_length);
		return 1;
	}

	printf("%s ok.\n", test_name);
	return 0;
}
	
int main()
{
	int failures = 0;
	printf("Starting tree generation models test...\n");
	failures += test_reciprocal_exponential_CDF();
	failures += test_tlt_grow_node();
	if (0 == failures) {
		printf("All tests ok.\n");
	} else {
		printf("%d test(s) FAILED.\n", failures);
		return 1;
	}

	return 0;
}
