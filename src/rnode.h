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

#include <stdbool.h>

struct rnode;

/** A node in a rooted tree. One of the basic building blocks of the whole
 * package. */

struct rnode {

	char *label;	/**< The node's label. May be "", but is never NULL. */
	/** The length of edge that leads to the parent (if any). Its value is
	 * numeric, and it can be zero or even negative (NJ trees), it can also
	 * be undefined (cladograms).  Therefore we store it as a string, with
	 * "" indicating an undefined length. This is how Newick does it
	 * anyway. */
	char *edge_length_as_string;	
	/** Used when we need the numerical value of the length
	 * (which is not very frequent, but does happen). */
	double edge_length;
	/** App-specific data. Any application-specific data (height, depth,
	 * etc) can be put into a structure which is pointed to by this
	 * member. See e.g. struct svg_data in svg_graph_common.h. */
	void *data;	

	struct rnode *parent;		/**< NULL iff root */
	/** NULL in leaves and root , normally not NULL otherwise */
	struct rnode *next_sibling;	
	int child_count;		/**< 0 iff leaf */
	struct rnode *first_child;	/**< NULL iff leaf */
	struct rnode *last_child;	/**< NULL iff leaf */

	/** Used by rnode_iterator to find next node to visit. */
	struct rnode *current_child;
	/** Used by rnode_iterator to mark visited nodes. */
	bool seen;	
};

/* allocates a rnode and returns a pointer to it, or exits. If 'label' is NULL
 * or empty, the node will have an empty string for a label. If
 * 'length_as_string' is NULL or empty, the node's branch will have no length
 * */
/* Returns NULL if structure cannot be created (malloc() problems). */

struct rnode *create_rnode(char *label, char *length_as_string);

/* Frees all rnode structures allocated so far. Use this after processing a
 * tree. */
// NOTE: for some reason it seems to make no difference whether or not this f()
// is called (according to Valgrind), even when running on several input trees.
// I conclude that my idea of a memory leak is incomplete.

void destroy_all_rnodes();

/* returns the number of children a node has. */

int children_count(struct rnode *node);

/* returns true IFF node is a leaf */

bool is_leaf(struct rnode *node);

/* returns true IFF node is root, which is defined to be one of i) node has
 * NULL parent edge, or ii) node has a parent edge, but this has a NULL parent
 * node. */

bool is_root(struct rnode *node);

/* Returns true IFF node is neither a leaf nor the root */

bool is_inner_node(struct rnode *node);

/* Returns true IFF i) node is not a leaf, and ii) all its children are leaves.
 * */

bool all_children_are_leaves(struct rnode *);

/* Returns true IFF i) node is not a leaf, and ii) all its children have the
 * same label. Sets 'label' to the common label (or to NULL if label isn't the
 * same in every child). */

bool all_children_have_same_label(struct rnode *, char **label);

/* Writes the node's address and label to stdout. Argument is void* so that
 * this function can be passed to dump_llist(). */

void dump_rnode(void *arg);

/* Returns true IFF node is a "stair" node */

bool is_stair_node(struct rnode *node);

/* Returns an array of pointers to all children, in Newick order. */

struct rnode** children_array(struct rnode *node);

/* Returns the list of nodes that descend from the argument node, in parse
 * order. Together with the argument node, this can be used to create a struct
 * rooted_tree. */
/* Returns NULL in case of malloc() problems. */

struct llist *get_nodes_in_order(struct rnode *);

/* Gets the number of rnodes in rnode_array. These are the rnodes created since
 * the beginning of the run, or since destroy_all_rnodes() was last called.
 * This is a testing function, not meant for app use (hence the leading '_').
 * */

int _get_rnode_count();

/* A debugging method that shows all nodes currently in rnode_array. */

void _show_all_rnodes();
