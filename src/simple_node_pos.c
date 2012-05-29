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
/*
#include <string.h>

#include "redge.h"
*/
#include "tree.h"
#include "simple_node_pos.h"
#include "list.h"
#include "rnode.h"
#include "common.h"

int alloc_simple_node_pos(struct rooted_tree *t) 
{
	struct list_elem *le;
	struct rnode *node;

	for (le = t->nodes_in_order->head; NULL != le; le = le->next) {
		node = le->data;
		node->data = malloc(sizeof(struct simple_node_pos));
		if (NULL == node->data) return FAILURE;
	}
	return SUCCESS;
}

void set_simple_node_pos_top (struct rnode * node, double top)
{
	((struct simple_node_pos *) node->data)->top = top;
}

void set_simple_node_pos_bottom (struct rnode * node, double bottom)
{
	((struct simple_node_pos *) node->data)->bottom = bottom;
}

void set_simple_node_pos_depth (struct rnode * node, double depth)
{
	((struct simple_node_pos *) node->data)->depth = depth;
}

double get_simple_node_pos_top (struct rnode * node)
{
	return ((struct simple_node_pos *) node->data)->top;
}

double get_simple_node_pos_bottom (struct rnode * node)
{
	return ((struct simple_node_pos *) node->data)->bottom;
}

double get_simple_node_pos_depth (struct rnode * node)
{
	return ((struct simple_node_pos *) node->data)->depth;
}

