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
/* order_tree.h: functions for ordering trees */

struct rooted_tree;


int order_tree_lbl(struct rooted_tree *);
int order_tree_num_desc(struct rooted_tree *);

/* A general-purpose sort function. Arguments are a tree, a node comparator
 * function, and a sort field setter function. The comparator is used by
 * qsort() to sort the children of a node, and the setter function sets the
 * node's sort field once its children have been sorted. See also
 * order_tree_lbl() and order_tree_num_desc(), which are
 * canned, easy-to-remember calls to this function. */

int order_tree(struct rooted_tree *tree,
		int (*comparator)(const void*,const void*),
		int (*sort_field_setter)(struct rnode *));

/* Comparators */

/* This compares node labels. */

int lbl_comparator(const void *, const void *);

/* Idem, but for ordering children by increasing number of descendants. Node
 * data must point to an int that holds the number of descendants. */

int num_desc_comparator(const void *a, const void *b);

/* This comparator is like num_desc_comparator(), but it reverses the
 * comparison every two nodes, breaking any ladder. */

int num_desc_deladderize(const void *a, const void *b);

/* Sort field setters. These return SUCCESS unless there was a memory problem.
 * */

/* Sets a node's sort field as the label of its first child */
int set_sort_field_label(struct rnode *);

/* Sets a node's sort field as the number of its descendants */
int set_sort_field_num_desc(struct rnode *);
