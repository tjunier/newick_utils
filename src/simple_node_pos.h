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
/* simple_node_pos.h: structures and functions for storing an rnode's position.
 * Used for displaying the tree as a graph, and computing node distances. */

struct rnode;
struct rooted_tree;

/** A very simple node position structure. Used for storing the node's vertical
 * and horizontal position - independent of final use (dispaly, distance
 * computations, etc). \a top and \a bottom are understood as if the tree were
 * displayed vertically: Vertical position is measured in leaf order or
 * fractions thereof (for inner nodes), \a depth is measured in edge length.
 * Function alloc_node_pos() should be used to allocate a struct node_pos for
 * each node of a tree, then functions set_node_depth_cb() and
 * set_node_vpos_cb() (see node_pos_alloc.h) should be used to compute and fill
 * in the structures' data.  Applications that only need to store nodes'
 * positions should use this structure. Other apps (like SVG display) who need
 * to store other per-node data should use another struct. */

struct simple_node_pos {
	double top;	/**< node's top position on graph */
	double bottom;	/**< node's bottom position on graph */
	double depth;	/**< node's depth in graph */
};

/* Allocates a simple_node_pos structure for each node. */
/* Returns FAILURE iff allocation fails. */

int alloc_simple_node_pos(struct rooted_tree *);

/* Setters and getters. These should be passed as arguments to
 * set_node_vpos_cb() and set_node_depth_cb() (see node_pos_alloc.h). */

void set_simple_node_pos_top (struct rnode *, double);
void set_simple_node_pos_bottom (struct rnode *, double);
void set_simple_node_pos_depth (struct rnode *, double);

double get_simple_node_pos_top (struct rnode *);
double get_simple_node_pos_bottom (struct rnode *);
double get_simple_node_pos_depth (struct rnode *);
