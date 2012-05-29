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
/* functions for tree models */

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <stdio.h>
#include <stdbool.h>

#include "list.h"
#include "rnode.h"
#include "common.h"
#include "link.h"
#include "to_newick.h"
#include "tree_models.h"
#include "masprintf.h"

#define UNUSED -1

static int running_number()
{
	static int n = 0;
	return n++;
}

/******************************************************************/
/* Geometric model */


/* Determines if a node is to have children, based on a pseudorandom number */

static bool geo_has_children(double prob_node_has_children)
{
	double rn = (double) rand() / RAND_MAX;

	if (rn <= prob_node_has_children)
		return true;
	else
		return false;
}

/* Visits a leaf: probabilistically adds children to the leaf, and adds those
 * children to the leaves queue (since they are new leaves) */

static int geo_visit_leaf(struct rnode *leaf, double prob_node_has_children,
		struct llist *leaves_queue)
{
	// printf ("visiting leaf %p (%s)\n", leaf, leaf->label);
	if (geo_has_children(prob_node_has_children)) {
		// printf (" gets children\n");
		struct rnode *kid1 = create_rnode("kid1", "");	
		if (NULL == kid1) return FAILURE;
		struct rnode *kid2 = create_rnode("kid2", "");	
		if (NULL == kid2) return FAILURE;
		add_child(leaf, kid1);
		add_child(leaf, kid2);
		if (! append_element(leaves_queue, kid1)) return FAILURE;
		if (! append_element(leaves_queue, kid2)) return FAILURE;
	} else {
		// printf (" gets no children\n");
	}

	return SUCCESS;
}

/* Generate a tree using the geometric model */

int geometric_tree(double prob_node_has_children)
{
	struct llist *leaves_queue = create_llist();
	if (NULL == leaves_queue) return FAILURE;
	struct rnode *root = create_rnode("root", "");
	if (NULL == root) return FAILURE;

	if (! append_element(leaves_queue, root)) return FAILURE;

	/* The queue contains any newly added leaves. We visit them in turn,
	 * possibly adding new leaves to the queue. The process stops when no
	 * new leaves have been added. */

	while (leaves_queue->count > 0) {
		int nb_leaves_to_visit = leaves_queue->count;
		/* Iterate over leaves. Note that new leaves can be added at
		 * the end of the queue */
		for (; nb_leaves_to_visit > 0; nb_leaves_to_visit--) {
			struct rnode *current_leaf = shift(leaves_queue);
			if (! geo_visit_leaf(current_leaf, prob_node_has_children,
					leaves_queue))
				return FAILURE;
		}
	}

	dump_newick(root);
	destroy_llist(leaves_queue);

	return SUCCESS;
}

/******************************************************************/
/* Time-limited model */

double _reciprocal_exponential_CDF(double x, double k)
{
	return - (log(1 - x)/k);
}

/* Returns a random number 0 <= rn < 1 */

double random_lt_1()
{
	double rn;
	do {
		rn = rand() / (double) RAND_MAX;
	} while (1.0 == rn);

	return rn;
}

/* "Grows" a node by setting a length (measured in time units) to its parent
 * edge. The length is randomly drawn from an exponential distribution, but it
 * cannot exceed a certain threshold, which must be stored in the node's data.
 * The function returns the remaining time (threshold - randomly drawn length)
 * If the function fails for some reason (e.g. no RAM left), it returns -1 */

double _tlt_grow_node(struct rnode *leaf, double branch_termination_rate,
		double alt_random)
{
	double rn;
       	if (alt_random < 0) {
		rn = random_lt_1();
	}
       	else
	       rn = alt_random;

	double length = _reciprocal_exponential_CDF(rn, 
			branch_termination_rate);

	/* The remaining time is the node's alloted time minus the branch
	 * length we just drew from the distribution */
	double alloted_time = *((double*)leaf->data);
	double remaining_time = alloted_time - length;

	/* Add remaining time if it's negative: this caps the branch at the
	 * time threshold. That way the tree is ultrametric, unless branches
	 * evolve at different rates. */
	if (remaining_time < 0)
		length += remaining_time;

	char *length_s = masprintf("%g", length);
	free(leaf->edge_length_as_string);
	leaf->edge_length_as_string = length_s;	/* NULL if masprintf() fails - check in caller */

	/* Return the remaining time so caller f() can take action based on
	 * whether there is time left or not */
	return remaining_time;
}

/* Creates a node with the specified time limit. */ 

struct rnode *create_child_with_time_limit(double time_limit)
{
	char *label = masprintf("n%d", running_number());
	if (NULL == label) 
		return NULL;
	struct rnode *kid = create_rnode(label, "");
	if (NULL == kid) return NULL;
	free(label);
	double *limit_p = malloc(sizeof(double));
	if (NULL == limit_p)
		return NULL;
	*limit_p = time_limit;

	kid->data = limit_p;

	return kid;
}

void free_data(struct llist *leaves_queue,
		struct rnode *root, struct llist *all_children)
{
	struct rnode *kid;

	destroy_llist(leaves_queue);

	//destroy_rnode(root, NULL);
	struct list_elem *elem;
	for (elem = all_children->head; NULL != elem; elem = elem->next) {
		kid = elem->data;
		free(kid->edge_length_as_string);
		free(kid->label);
	       	free(kid->data);
		free(kid);
	}
	destroy_llist(all_children);
}

int time_limited_tree(double branch_termination_rate, double duration)
{
	/* create root */
	char *label = masprintf("n%d", running_number());
	if (NULL == label) { return FAILURE; }
	struct rnode *root = create_rnode(label, "");
	free(label);

	/* This list remembers all children nodes created, for purposes of
	 * freeing */
	struct llist *all_children = create_llist();
	if (NULL == all_children) return FAILURE;

	/* This list stores children who have yet to grow (and maybe have
	 * children of their own) */
	struct llist *leaves_queue = create_llist();
	if (NULL == leaves_queue) return FAILURE;
	struct rnode *kid;

	/* 1st child */
	kid = create_child_with_time_limit(duration);
	if (NULL == kid) return FAILURE;
	/* add_child(...): length is determined below */
	add_child(root, kid);
	if (! append_element(leaves_queue, kid)) return FAILURE;
	if (! append_element(all_children, kid)) return FAILURE;

	/* 2nd child */
	kid = create_child_with_time_limit(duration);
	if (NULL == kid) return FAILURE;
	add_child(root, kid);
	if (! append_element(leaves_queue, kid)) return FAILURE;
	if (! append_element(all_children, kid)) return FAILURE;


	while (0 != leaves_queue->count) {
		struct rnode *current = shift(leaves_queue);
		double remaining_time = _tlt_grow_node(current,
				branch_termination_rate, UNUSED); 
		/* length is set by tlt_grow_node(), NULL means error */
		if (NULL == current->edge_length_as_string)
			return FAILURE;
		if (remaining_time > 0) {
			kid = create_child_with_time_limit(remaining_time);
			if (NULL == kid) return FAILURE;
			add_child(current, kid);
			if (! append_element(leaves_queue, kid)) return FAILURE;
			if (! append_element(all_children, kid)) return FAILURE;

			kid = create_child_with_time_limit(remaining_time);
			if (NULL == kid) return FAILURE;
			add_child(current, kid);
			if (! append_element(leaves_queue, kid)) return FAILURE;
			if (! append_element(all_children, kid)) return FAILURE;
		} 
	}

	dump_newick(root);

	free_data(leaves_queue, root, all_children);

	return SUCCESS;
}
