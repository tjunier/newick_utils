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

/* The functions in this module provide an iterator interface on a node and its
 * descendents, allowing iterative traversal. The low-level rnode_iterator and
 * functions that call it directly (such as rnode_iterator_next()) visit the
 * tree by following edges (depth first, and visiting each child node in
 * order). All nodes except leaves are thus visited more than once.
 * Higher-level functions (like get_nodes_in_order()) can discard
 * already-visited nodes and produce e.g. post-order traversals, etc. */

/* In general, there is no need to use these functions because most operations
 * can be done using a tree's 'nodes_in_order' list. Looping on this list will
 * be faster than iterating on a subtree. But there are exceptions:

 o the 'nodes_in_order' list may be outdated (e.g. because nodes were inserted,
   deleted, etc) - in that case, the list should be reconstructed with
   get_nodes_in_order(), which uses the rnode iterator.
 
 o the 'nodes_in_order' list may not contain all the needed information (this
   is the case when outputting Newick).

 */

/* The iterator relies on members 'seen' and 'current_child' of struct rnode.
 * This avoids the need for the iterator itself to keep track of the nodes it
 * has already seen, but of course it also means that at any given time only
 * one iterator can be traversing any clade. This seems not to be a problem,
 * and it is much faster (the first version of rnode_iterator kept track of
 * seen nodes with a hash table, and it was significantly slower. */

#include <stdbool.h>

struct rnode;
struct llist;
struct rooted_tree;

struct rnode_iterator;

/* Creates an iterator. You can then pass it to the functions below. */
/* Returns NULL in case of problems (which will be with allocation). Do NOT use
 * more than one iterator simultaneously on the same tree/subtree (see above).
 * */

struct rnode_iterator *create_rnode_iterator(struct rnode *root);

/** Gets the iterators's root 
 *
 * \param it the iterator
 * \return its root
 */

struct rnode *get_rnode_iterator_root(struct rnode_iterator*);

/* Destroys the iterator, freeing allocated memory */

void destroy_rnode_iterator(struct rnode_iterator *);

/* Gets the next node. Returns NULL if there is no more nodes, or in case of
 * error. Sets the 'status' flag of the iterator to indicate the reason for
 * NULL.  */

struct rnode *rnode_iterator_next(struct rnode_iterator *);

/* Returns true IFF there are more children to visit on the current node. */

bool more_children_to_visit (struct rnode_iterator *);
