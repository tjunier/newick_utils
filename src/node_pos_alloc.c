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
/* text_graph.c - functions for drawing trees on a text canvas. */

#include <stdlib.h>
#include <string.h>
/*
#include <stdio.h>

*/
#include "rnode.h"
#include "tree.h"
#include "list.h"
#include "node_pos_alloc.h"
#include "common.h"

int set_node_vpos_cb(struct rooted_tree *t,
		void (*set_node_top)(struct rnode *, double),
		void (*set_node_bottom)(struct rnode *, double),
		double (*get_node_top)(struct rnode *),
		double (*get_node_bottom)(struct rnode *))
{
	int leaf_count = 0;
	struct list_elem *le;
	struct rnode *node;

	for (le = t->nodes_in_order->head; NULL != le; le = le->next) {
		node = le->data;
		if(is_leaf(node)) {
			set_node_top(node, leaf_count);
			set_node_bottom(node, leaf_count);
			leaf_count++;
		} else {
			struct rnode *top_child, *bottom_child;
			/* top of this node is average of top and bottom of its
			 * top child node */
		       	top_child = node->first_child;
			set_node_top(node, 0.5 * (
					get_node_top(top_child) +
					get_node_bottom(top_child))
					);
			/* same idea for bottom */
		       	bottom_child = node->last_child;
			set_node_bottom(node, 0.5 * (
					get_node_top(bottom_child) +
					get_node_bottom(bottom_child))
					);
		}
	}

	return leaf_count;
}

struct h_data set_node_depth_cb(struct rooted_tree *tree,
		void (*set_node_depth)(struct rnode *, double),
		double (*get_node_depth)(struct rnode *))
{
	struct llist *nodes_in_reverse_order;
	struct list_elem *elem;
	struct rnode *node;
	int max_label_len = 0;
	double max_leaf_depth = 0.0;
	struct h_data result;
	result.status = FAILURE; 

	nodes_in_reverse_order = llist_reverse(tree->nodes_in_order);
	if (NULL == nodes_in_reverse_order) return result; /* fails! */

	/* set the root's depth to 0 */
	elem = nodes_in_reverse_order->head;
	node = (struct rnode *) elem->data;
	set_node_depth(node, 0.0);

	/* now traverse node list, setting each node's depth to the sum of its
	 * parent edge's length and its parent node's depth. */
	elem = elem->next;
	for (; NULL != elem; elem = elem->next) {
		node =  elem->data;
		struct rnode *parent_node = node->parent;

		if (0 == strcmp("", node->edge_length_as_string))
			node->edge_length = 1.0;
		else
			node->edge_length = atof(node->edge_length_as_string);

		double node_depth = node->edge_length +
			get_node_depth(parent_node);

		set_node_depth(node, node_depth);
		
		/* Update max depth and max label length */
		if (node_depth > max_leaf_depth) {max_leaf_depth=node_depth;}
		if (is_leaf(node) && (NULL != node->label)) {
			int lbl_len = strlen(node->label);
			if (lbl_len > max_label_len) {
				max_label_len = lbl_len;
			}
		}
	}
	destroy_llist(nodes_in_reverse_order);

	result.l_max = max_label_len;
	result.d_max = max_leaf_depth;
	result.status = SUCCESS;

	return result;
}
