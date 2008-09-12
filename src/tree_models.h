/* Different models of tree generation */

/* A pretty simple model where each node has a fixed probability of having 2
 * children. If set to > 0.5, the expected number of leaves is infnite and the
 * program will probably not stop of its own. */

void geometric_tree(double prob_node_has_children);

/* A more complicated model where each branch's length is exponentially distributed (up to a duration threshold).  */

void time_limited_tree(double branch_termination_rate, double duration);
