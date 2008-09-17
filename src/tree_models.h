/* Different models of tree generation */

struct rnode;

/* A pretty simple model where each node has a fixed probability of having 2
 * children. If set to > 0.5, the expected number of leaves is infnite and the
 * program will probably not stop of its own. */

void geometric_tree(double prob_node_has_children);

/* A more complicated model where each branch's length is exponentially distributed (up to a duration threshold).  */

void time_limited_tree(double branch_termination_rate, double duration);

/* Attributes length to the parent edge (exponentially distributed with
 * parameter 'branch_termination_rate'), capped by duration threshold stored in
 * node's data pointer. Returns the remaining time (which can be negative) The
 * 'alt_random' parameter is used (if > 0) for testing the function by
 * supplying a known "random" value */

double tlt_grow_node(struct rnode *, double branch_termination_rate,
		double alt_random);

/* The reciprocal of an exponential CDF with parameter k, used for sampling a
 * value from an exponential PDF using a random number 0 <= x < 1. */

double reciprocal_exponential_CDF(double x, double k);
