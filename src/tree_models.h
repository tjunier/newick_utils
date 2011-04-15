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

/** \file
 * Different models of tree generation */

struct rnode;

/** Prints a random tree grown using a geometric model. A pretty simple model
 * where each node has a fixed probability of having 2 children. If set to >
 * 0.5, the expected number of leaves is infnite and the program will probably
 * not stop of its own. 
 \arg \c prob_node_has_children the probability of node having (2) children
 \return FAILURE if there was a problem (most probably malloc()) 
 \todo: should return the tree instead of directly dumping it.
 */

int geometric_tree(double prob_node_has_children);

/** Prints a random time-limited tree. A more complicated model (WRT
 * geometric_tree())  where each branch's length is exponentially distributed
 * (up to a duration threshold).  The 1st parameter is the
 * exponential distribution's rate parameter, which is the inverse of the mean. 
 * \arg \c branch_termination_rate see text
 * \arg \c duration see text
 * \return FAILURE (0) IFF there is any problem (which will be memory
 * allocation errors). 
 \todo: should return the tree instead of directly dumping it.  */

int time_limited_tree(double branch_termination_rate, double duration);

/** \cond -- The following functions are public only so they can be tested;
 * they should not be used outside tree_models.c (hence the leading _ in their
 * names). */

/**  Attributes length to the parent edge (exponentially distributed with
 * parameter 'branch_termination_rate'), capped by duration threshold stored in
 * node's data pointer. Returns the remaining time (which can be negative) The
 * 'alt_random' parameter is used (if > 0) for testing the function by
 * supplying a known "random" value.
 * If the function fails for some reason (e.g. no RAM left), it returns -1. 
 * Note: we only keep this function public so it can be tested, hence the
 * leading  _. */

double _tlt_grow_node(struct rnode *, double branch_termination_rate,
		double alt_random);


/** The reciprocal (i.e., quantile function) of an exponential CDF with
 * parameter k, used for sampling a value from an exponential PDF using a
 * (uniformly distributed) random number 0 <= x < 1.
 * \arg \c k parameter to the exponential CDF
 * \arg \c x the random number used to draw from the CDF
 * \return a random value from an exponential distribution */

double _reciprocal_exponential_CDF(double x, double k);

/** \endcond */
