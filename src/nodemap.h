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
/* Functions for creating and searching label->node maps */

struct rnode;
struct hash;
struct llist;

/* Given a list of nodes (e.g., tree->nodes_in_order), creates a map of all
 * nodes, keyed by label. Empty labels are ignored. Labels should be unique.
 * Redundant labels will not cause a crash, but may cause unexpected results as
 * the "map" will not be one-to-one. */
/* Returns NULL in case of malloc() error. */

struct hash *create_label2node_map(const struct llist *);

/* Given a list of nodes (e.g., tree->nodes_in_order), creates a map of _lists_
 * of nodes of the same label - this means we can handle tree with nonunique
 * labels (cf create_label2node_map() which assumes labels are unique). Empty
 * labels are treated like any other label. Nodes of the same label are stored
 * in the order they are in the parameter list. */
/* Returns NULL in case of malloc() error. */

struct hash *create_label2node_list_map(const struct llist *);

/* Destroys a label->node list map such as those created by
 * create_label2node_list_map() */

void destroy_label2node_list_map(struct hash *);

/* Returns a label->rnode map of all leaves that descend from 'root' */
/* Returns NULL in case of malloc() problems. */

struct hash *get_leaf_label_map_from_node(struct rnode *root);
