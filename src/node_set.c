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
 * nodes, we need to allocate a contiguous array of bits. Fails if char is more
 * than 1 byte long (very unlikely, but who knows?) 'node_count' is the total
 * number of nodes, and hence of bits in the field; 'node_number' is the number
 * of the bit which must be set (the others must be zero). This represence the
 * presence of that particular node. */

node_set create_node_set(int node_number, int node_count)
{
	node_set set;
	int node_byte;	/* which byte contains the bit for the node*/
	int node_bit;	/* which bit represents the node */

	/* sanity checks */
	if (node_number < 0 || (node_number >= node_count)) { return NULL; }

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

	/* Now we need to find which bit to set, in which byte */
	node_byte = node_number / BYTE_SIZE;
	node_bit = node_number % BYTE_SIZE;

	set[node_byte] = (1 << node_bit);

	return set;
}

int is_set(node_set set, int n)
{
	int node_byte = n / BYTE_SIZE;
	int node_bit = n % BYTE_SIZE;
	
	return set[node_byte] & (1 << node_bit);
}

node_set test_set_union(node_set set1, node_set set2, int node_number)
{
	node_set result;

	/* TODO: it is now impossible to create an empty nodeset. Separate
	 * creation from setting by creating a new function node_set_add().
	 * Also, rename is_set() to set_contains() - too much confusion from
	 * using 'set' in two different meanings (as in "setting a bit to 1"
	 * and a "set of nodes"). */
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

struct llist *children_node_set_list(struct rnode *node, struct hash *name2num,
		int count)
{
	return NULL;
}
