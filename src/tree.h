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
#include <regex.h>
#include <stdbool.h>

struct rnode;
struct llist;

extern const int FREE_NODE_DATA;
extern const int DONT_FREE_NODE_DATA;

enum tree_type {TREE_TYPE_UNKNOWN, TREE_TYPE_CLADOGRAM, TREE_TYPE_PHYLOGRAM, TREE_TYPE_NEITHER};


/** The struct used for rooted trees. 
 \todo: would probably be a good idea to make this private and use accessors.
 */

struct rooted_tree {
	struct rnode *root;		/**< tree's root */
	struct llist *nodes_in_order;	/**< llist of nodes, in postorder */
	enum tree_type type;		/**< see enum tree_type */
};

/* Reroots the tree in such a way that 'outgroup' and descendants are one of
 * the root's children, and the rest of the tree is the other child. The old
 * root node gets spliced out if it has only one child. */
/* Returns SUCCESS unless there was a problem (malloc()) */

int reroot_tree(struct rooted_tree *tree, struct rnode *outgroup);

/* Collapses pure clades (= clades in which all leaves are of the same label)
 * into a single leaf, also of the same label */

void collapse_pure_clades(struct rooted_tree *tree);

/* Destroys a tree, releasing memory. */

void destroy_tree(struct rooted_tree *);

/* Returns the number of leaves of this tree */

int leaf_count(struct rooted_tree *);

/* Returns a list of nonempty leaf labels, in parse order */

struct llist *get_leaf_labels(struct rooted_tree *);

/* Returns a list of all nonempty labels, in parse order */

struct llist *get_labels(struct rooted_tree *);

/* Returns true iff tree is a cladogram, i.e. doesn't contain any branch length
 * information. A single nonempty branch length means that the tree is NOT a
 * cladogram (although I don't think that such a tree is very elegant).
 * Likewise, a tree with all branch lengths set to zero isn't a cladogram
 * either (exactly what it is, I don't know :-) ) */

bool is_cladogram(struct rooted_tree *tree);

/* Takes a list of labels and returns the corresponding nodes, in the same
 * order */

struct llist *nodes_from_labels(struct rooted_tree *tree,
		struct llist *labels);

/* like nodes_from_labels(), but uses a regular expression (compiled from second argument) instead of a list of labels. */
/* NOTE: if you use the same regexp for >1 calls, try nodes_from_regexp()
 * below and compile the regexp only once beforehand (and free it afterwards). */
/* Returns NULL on failure. */

struct llist *nodes_from_regexp_string(struct rooted_tree *tree,
		char *regexp_string);

/* like nodes_from_labels(), but uses a regular expression (passed as second argument) instead of a list of labels. */
/* Returns NULL on failure. */

struct llist *nodes_from_regexp(struct rooted_tree *tree,
		regex_t *preg);

/* Clones a (sub)tree, given the root node of the subtree. All nodes and edges
 * are new: one can modify or delete the clone without affecting the original
 * in any way. */
/* Returns NULL on failure. */

struct rooted_tree *clone_subtree(struct rnode *);

/* Returns the tree's type (Cladogram, Phylogram, etc) */
enum tree_type get_tree_type(struct rooted_tree *tree);
