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

#include "node_set.h"
#include "tree.h"
#include "rnode.h"
#include "list.h"
#include "hash.h"

const int BYTE_SIZE = 8;

/* We implement a node set as a bit field. Since there can be thousands of
 * nodes, we need to allocate a contiguous array of bits. Argument 'node_count'
 * is the total number of nodes in the tree, and hence of bits in the field.
 * Since it is constant for any given tree, we do not store it within the
 * node_set, but pass it to the functions. This requires less storage, and
 * allows us to free the node_sets directly, since they are not structures. */


/* Fails if the tree has 0 nodes */

node_set create_node_set(int node_count)
{
	node_set set;
	
	/* First, we need to see how many bytes we will need. */
	int num_bytes = node_count / BYTE_SIZE;
       	/* if the remainder is not null, add one char */
	if (node_count % BYTE_SIZE != 0) num_bytes++;

	/* allocate bytes and clear them */
	assert(num_bytes != 0);
	set = malloc(num_bytes);
	if (NULL == set) return NULL;
	memset(set, 0, num_bytes);
	// printf ("[cns] created node set %p\n", set);

	return set;
}

void node_set_add(node_set set, int node_number, int node_count)
{
	int node_byte;	/* which byte contains the bit for the node*/
	int node_bit;	/* which bit represents the node */

	/* sanity checks */
	assert(node_count > 0);
	assert(node_number >= 0);
	assert(node_number < node_count);

	/* Now we need to find which bit to set, in which byte */
	node_byte = node_number / BYTE_SIZE;
	node_bit = node_number % BYTE_SIZE;

	set[node_byte] = (1 << node_bit);
}

int node_set_contains(node_set set, int node_number, int node_count)
{
	int node_byte = node_number / BYTE_SIZE;
	int node_bit = node_number % BYTE_SIZE;

	/* sanity checks */
	assert(node_count > 0);
	assert(node_number >= 0);
	assert(node_number < node_count);
	
	return set[node_byte] & (1 << node_bit);
}

node_set node_set_union(node_set set1, node_set set2, int node_count)
{
	node_set result;
	int num_bytes;
	int i;

	assert(node_count > 0);
	result = create_node_set(node_count);
	if (NULL == result) return NULL;

	num_bytes = node_count / BYTE_SIZE;
	if (node_count % BYTE_SIZE != 0) { num_bytes++; }

	for (i = 0; i < num_bytes; i++) {
		result[i] = set1[i] | set2[i];
	}

	return result;
}

void node_set_add_set(node_set set1, node_set set2, int node_count)
{
	int num_bytes;
	int i;

	assert(node_count > 0);

	num_bytes = node_count / BYTE_SIZE;
	if (node_count % BYTE_SIZE != 0) { num_bytes++; }

	for (i = 0; i < num_bytes; i++) {
		set1[i] |= set2[i];
	}
}

int build_name2num(struct rooted_tree *tree, struct hash **name2num_ptr)
{
	/* If the tree is dichotomous and has N nodes, then it has L = (N+1)/2
	 * leaves. But for highly polytomous trees, L is higher, and can
	 * approach N in some cases (e.g. when most leaves are attached to the
	 * root due to low bootstrap support). So we allocate N bins. */
	struct hash *n2n = create_hash(tree->nodes_in_order->count);
	if (NULL == n2n) return NS_MEM_ERROR;
	struct list_elem *el;
	int ord_number = 0;

	for (el = tree->nodes_in_order->head; NULL != el; el = el->next) {
		struct rnode *current = (struct rnode *) el->data;
		if (is_leaf(current)) {
			int *nump;
			if (strcmp("", current->label) == 0)
				return NS_EMPTY_LABEL;
			if (NULL != hash_get(n2n, current->label)) 
				return NS_DUP_LABEL;
			nump = malloc(sizeof(int));
			if (NULL == nump) 
				return NS_MEM_ERROR;
			*nump = ord_number;
			if (! hash_set(n2n, current->label, nump))
				return NS_MEM_ERROR;
			ord_number++;
		}
	}	
	*name2num_ptr = n2n;

	return NS_OK;
}

char *node_set_to_s(node_set set, int node_count)
{
	char *result;
	int i;

	result = malloc ((node_count + 1) * sizeof(char));
	if (NULL == result) return NULL;

	for (i = 0; i < node_count; i++) {
		int node_byte = i / BYTE_SIZE;
		int node_bit = i % BYTE_SIZE;
		if (set[node_byte] & (1 << node_bit)) {
			result[i] = '*';
		} else {
			result[i] = '.';
		}
	}
	result[i] = '\0';

	return result;
}
