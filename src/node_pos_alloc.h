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

/** \file node_pos_alloc.h: functions for allocating node's positions. Used
 * for displaying and computing node distances. */

struct rooted_tree;
struct rnode;

/** used for returning max depth and max label length */

struct h_data {
	int l_max; 	/**< lenth of longest label */
	double d_max; 	/**< depth of deepest leaf */
	int status;	/**< used to indicate errors */
};

/* \par The following functions set the value of the node's positions. This is
 * done using callback functions. The advantage of this is that the functions
 * declared here do not need to know how this data is stored (it may be stored
 * in a struct attributed to rnode->data (and it can be a different structure
 * for different applications); it might be a hash, or whatever). As a
 * consequence, these functions can be used by different applications, which
 * avoids code duplication. The downside, of course, is that accessors must be
 * supplied, and a few more function calls have to be executed. */

/** Sets the nodes' vertical position, which is a dimensionless number based on
 * the ordinal number of leaves (i.e. first leaf is 1, second is 2, their
 * ancestor is 1.5 (unless it has other children), etc). Last four arguments
 * are setters and getters for an rnode's top and bottom. Returns the number of
 * leaves in the tree (useful for computing scale). */ 

int set_node_vpos_cb(struct rooted_tree *t,
		void (*set_node_top)(struct rnode *, double),
		void (*set_node_bottom)(struct rnode *, double),
		double (*get_node_top)(struct rnode *),
		double (*get_node_bottom)(struct rnode *));

/** Sets the nodes' horizontal position (depth). This is determined by parent
 * edges's lengths. Edges with empty length (NOT zero length!) are arbitrarily
 * attributed a length of 1. The last 2 arguments are a setter and a getter for
 * node depth (see note above). Returns a structure containing the tree's depth
 * (depth of the deepest leaf) and the length (in characters) of the longest
 * label. These are useful for determining horizontal scale.  In case of error,
 * the structure's 'status' member is set to FAILURE, otherwise it is SUCCESS.
 * */

struct h_data set_node_depth_cb(struct rooted_tree *,
		void (*set_node_depth)(struct rnode *, double),
		double (*get_node_depth)(struct rnode *));
