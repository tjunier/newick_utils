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
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "rnode_iterator.h"
#include "hash.h"
#include "list.h"
#include "rnode.h"
#include "tree.h"

/* Some debugging macros. Compile with -DDEBUG_ITERATOR and the addresses and
 * labels of the next nodes (as returned by rnode_iterator_next()) will be
 * printed. */

#ifdef DEBUG_ITERATOR
#define SHOW printf ("-> %p\t%s\n", iter->current, iter->current->label)
#define SHOW_END printf ("-> END\n")
#else
#define SHOW ;
#define SHOW_END ;
#endif

/* The functions in this module provide an iterator interface on a node and its
 * descendents, allowing traversal. The low-level rnode_iterator and functions
 * that call it directly (such as rnode_iterator_next()) visit the tree by
 * following edges (depth first, and visiting each child node in order). All
 * nodes except leaves are thus visited more than once. Higher-level functions
 * (like get_nodes_in_order()) can discard already-visited nodes and produce
 * e.g. post-order traversals, etc. */

/* In general, there is no need to use these functions because most operations
 * can be done using a tree's 'nodes_in_order' list. Looping on this list will
 * be faster than iterating on a subtree. But there are exceptions:

 o the 'nodes_in_order' list may be outdated (e.g. because nodes were inserted,
   deleted, etc) - in that case, the list should be reconstructed with
   get_nodes_in_order(), which uses the rnode iterator.
 
 o the 'nodes_in_order' list may not contain all the needed information (this
   is the case when outputting Newick).

 */

/* NOTE: functions in this module now rely on the 'seen' member of struct
 * rnode. The former approach, namely to 'remember' visited nodes with hash
 * tables, proved too slow. It is implicitly assumed that the 'seen' member of
 * each node in the tree is zero before the functions are called. You can use
 * the convenience function reset_seen() for this. */

/* Another possible imlementation would be to use a stack of nodes to visit,
 * and processing the top node until the statck is empty. Processing would mean
 * pushing all the node's children, then removing the node. But I'm not sure it
 * would be faster than using 'seen' flags. */


struct rnode_iterator
{
	struct rnode *root;	/**< starting point */
	struct rnode *current;	/**< currently visited node */
};

/* see note above about the 'seen' member of struct rnode */

struct rnode_iterator *create_rnode_iterator(struct rnode *root)
{
	struct rnode_iterator *iter;
	iter = (struct rnode_iterator *) malloc(sizeof(struct rnode_iterator));
	if (NULL == iter) return NULL;

	iter->root = iter->current = root;

	return iter;
}

struct rnode *get_rnode_iterator_root(struct rnode_iterator* it)
{
	return it->root;
}

void destroy_rnode_iterator (struct rnode_iterator *it)
{
	free(it);
}

/* Returns true IFF there are more children to visit on the current node. */

bool more_children_to_visit (struct rnode_iterator *iter)
{
	if  (iter->current->current_child != iter->current->last_child)
		return true;
	else
		return false;
}

/* The iterator keeps state in member 'current'. This is a rnode, and it uses
 * its 'current_child' member to keep track of which of its children have
 * already been visited. Before a node is visited for the first time, its
 * 'current_child' is NULL, then it visits all children in turn. This function
 * thus needs to update two things: i) the current node in the iterator, and
 * ii) the current child in the current node. */

struct rnode *rnode_iterator_next(struct rnode_iterator *iter)
{
	/* We have to consider the case of a single-node tree (this happens
	 * e.g. in nw_match if none of the labels in the pattern tree is found
	 * in the target tree). The single node is in this case both a leaf (no
	 * children) and the root (no parent). Hence the double test below. */

	/* Case 1: we're on a leaf */
	if (is_leaf(iter->current)) {
		if (iter->current == iter->root) {
			/* The (very) special case of a root leaf. The only
			 * case when a leaf is visited twice. we use
			 * current_child in a different way here - this is a
			 * leaf, so it obviously has no children.
			 * current_child_elem is set to the current node the
			 * first time, and reset to NULL the second time. This
			 * allows the iterator to return a leaf when and only
			 * when it starts on a leaf. */
			struct rnode dummy;
			if (NULL == iter->current->current_child) {
				iter->current->current_child = &dummy;
					SHOW;
				return iter->current;
			} else {
				iter->current->current_child = NULL;
				SHOW_END;
				return NULL;
			}
		} else {
			/* we're not on the iterator's root, so we return the
			 * leaf's parent. */
			iter->current = iter->current->parent;
			SHOW;
			return iter->current;
		}
	}

	/* Case 2: we're on an inner node, possibly the root. In any case, the
	 * node has children. We see if we visited them all (in which case we
	 * go to the parent) or not (in which case we go to the first unvisited
	 * child).  Variable current_child points to a node's currently visited
	 * child.*/
	if (iter->current->current_child
	    == iter->current->last_child) {	/* seen all children */
		iter->current->current_child = NULL; /* reset */
		if (iter->root == iter->current) {
			SHOW_END;
			return NULL;	/* done iterating */
		} else {
			iter->current = iter->current->parent;
			SHOW;
			return iter->current;
		}
	}

	/* More children to visit... */
	if (NULL == iter->current->current_child) 
		// first time on this node
		iter->current->current_child = iter->current->first_child;
	else
		iter->current->current_child =
			iter->current->current_child->next_sibling;

	iter->current = iter->current->current_child;	
	SHOW;
	return iter->current;
}
