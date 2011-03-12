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
/* Functions for node sets, and related ancillary tasks. */

struct llist;
struct hash;
struct rnode;
struct rooted_tree;

/* Error codes */

enum ns_return {NS_OK, NS_DUP_LABEL, NS_EMPTY_LABEL, NS_MEM_ERROR};

/* I rarely use typedefs, but in this case I think it makes f() signatures
 * easier to read. */

typedef char* node_set;

/* Creates a node_set for 'node_count' nodes. */
/* Returns NULL in case of malloc() error. */

node_set create_node_set(int node_count);

/* Adds node 'node_number' to set */

void node_set_add(node_set set, int node_number, int node_count);

/* returns true iff 'set' contains node number 'node_number' */

int node_set_contains(node_set set, int node_number, int node_count);

/* returns the union of two sets, or NULL in case of malloc() problems */

node_set node_set_union(node_set set1, node_set set2, int node_count);

/* adds set2 to set1 (which is modified) */

void node_set_add_set(node_set set1, node_set set2, int node_count);

/* Creates a label -> ordinal number map.  Returns 0 if there was a problem
 * (such as a leaf without a label, or a non-unique label; returns 1 otherwise
 * */

int build_name2num(struct rooted_tree *tree, struct hash **name2num_ptr);

/* returns a string representation of the node set (cf PHYLIP) */

char * node_set_to_s(node_set set, int node_count);
