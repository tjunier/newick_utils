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
	if (node_count % BYTE_SIZE != 0) { num_bytes++; }

	/* allocate bytes and clear them */
	assert(num_bytes != 0);
	set = malloc(num_bytes);
	if (NULL == set) {
		perror(NULL);
		exit(EXIT_FAILURE);
	}
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
			if (NULL == nump) {
				perror(NULL);
				exit(EXIT_FAILURE);
			}
			*nump = ord_number;
			hash_set(n2n, current->label, nump);
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
	if (NULL == result) {
		perror(NULL);
		exit(EXIT_FAILURE);
	}

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
