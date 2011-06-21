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
struct rooted_tree;
struct rnode;
struct llist;

/* Given a tree and two nodes, returns their last common ancestor.
 * NOTE: Both nodes are assumed to belong to the tree; if this is
 * not the case, then the function will return the tree's root or
 * may hang.  NOTE: Clobbers node->data. */
/* Returns NULL in case of malloc() error. */

struct rnode *lca2(struct rooted_tree *, struct rnode *,
		struct rnode *);

/* Given a tree and a list of nodes, returns the LCA */

struct rnode *lca_from_nodes(struct rooted_tree *tree, struct llist *labels);

/* Given a tree and a list of labels, returns the LCA (assumes labels are
unique in tree - use lca_from_labels_multi() if labels are not known to be
unique) - NOTE: there is probably no reason to use this function rather than
the newer one that can handle multiple nodes with identical labels. Maybe it is
a little faster. I keep it for compatibility.  */

struct rnode *lca_from_labels(struct rooted_tree *tree, struct llist *labels);

/* Given a tree and a list of labels, returns the LCA (does not assumes that
labels unique in tree)  */

struct rnode *lca_from_labels_multi(struct rooted_tree *tree, struct llist *labels);
