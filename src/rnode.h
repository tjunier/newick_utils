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

struct rnode {

	/* When we speak of 'the' edge of a node, we mean the edge that leads
	 * to its parent. Its value is numeric, and it can be zero or even
	 * negative (NJ trees), it can also be undefined (cladograms).
	 * Therefore we store it as a string, with "" indicating an undefined
	 * length. This is how Newick does it anyway. */
	char *edge_length_as_string;	
	double edge_length; /* TODO: check if we use this. */
	char *label;
	void *data;	/* app-dependent data for this node */

	struct rnode *parent;		/* NULL iff root */
	struct rnode *next_sibling;
	int child_count;		/* 0 iff leaf */
	struct rnode *first_child;	/* NULL iff leaf */
	struct rnode *last_child;	/* NULL iff leaf */

	/* enables traversing by rnode_iterator_next() */
	struct rnode *current_child;
	int seen;	// TODO: check if we could use a boolean.
};

/* allocates a rnode and returns a pointer to it, or exits. If 'label' is NULL
 * or empty, the node will have an empty string for a label. If
 * 'length_as_string' is NULL or empty, the node's branch will have no length
 * */
/* Returns NULL if structure cannot be created (malloc() problems). */

struct rnode *create_rnode(char *label, char *length_as_string);

/* Destroys the rnode, including the parent edge (if any). If free_data is not
 * NULL, it is called to free the node's data */

void destroy_rnode(struct rnode *node, void (*free_data)(void*));

/* returns the number of children a node has. */

int children_count(struct rnode *node);

/* returns true IFF node is a leaf */

int is_leaf(struct rnode *node);

/* returns true IFF node is root, which is defined to be one of i) node has
 * NULL parent edge, or ii) node has a parent edge, but this has a NULL parent
 * node. */

int is_root(struct rnode *node);

/* Returns true IFF node is neither a leaf nor the root */

int is_inner_node(struct rnode *node);

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

int is_stair_node(struct rnode *node);

/* Frees a node and all its descendants. Use this when you cannot use
 * destroy_tree(), e.g. when the tree structure has been altered. */ 
/* Like all free()ing functions, does not return error codes. */

void free_descendants(struct rnode *node);

/* Returns an array of pointers to all children, in Newick order. */

struct rnode** children_array(struct rnode *node);

/* Returns the list of nodes that descend from the argument node, in parse
 * order. Together with the argument node, this can be used to create a struct
 * rooted_tree. */
/* Returns NULL in case of malloc() problems. */

struct llist *get_nodes_in_order(struct rnode *);
