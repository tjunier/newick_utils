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

/* A helper function, to pass as parameter to qsort(). This compares node labels. */

int lbl_comparator(const void *, const void *);

/* Idem, but for ordering children by increasing number of descendants */

int num_desc_comparator(const void *a, const void *b);

/* reverse of num_desc_comparator() */

int reverse_num_desc_comparator(const void *a, const void *b);

/* Orders all nodes in the tree, according to lbl_comparator() - this could be
 * parametered, if we can think of other interesting orders. NOTE: this
 * function uses (and modifies) the rnode's data attribute. */

int order_tree_lbl(struct rooted_tree *tree);

/* Idem, but orders according to number of descendants. */

int order_tree_num_desc(struct rooted_tree *tree);

/* Idem, but de-ladderizes */

int order_tree_deladderize(struct rooted_tree *tree);
