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
/* functions for iterating over a rnode hierarchy given its root, and utility
 * functions that depend on those */

struct rnode;
struct rnode_iterator;
struct llist;

/* Creates an iterator. You can then pass it to the functions below. */
/* Returns NULL in case of problems (which will be with allocation) */

struct rnode_iterator *create_rnode_iterator(struct rnode *root);

/* Destroys the iterator, freeing allocated memory */

void destroy_rnode_iterator(struct rnode_iterator *);

/* Gets the next node information */

struct rnode *rnode_iterator_next(struct rnode_iterator *);

/* TODO: these are client functions, they may belong elsewhere */

/* Returns the list of nodes that descend from the argument node, in parse
 * order. Together with the argument node, this can be used to create a struct
 * rooted_tree. */
/* Returns NULL in case of malloc() problems. */

struct llist *get_nodes_in_order(struct rnode *);

/* Returns a label->rnode map of all leaves that descend from 'root' */
/* Returns NULL in case of malloc() problems. */

struct hash *get_leaf_label_map(struct rnode *root);
