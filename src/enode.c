#include <stdlib.h>
#include <stdio.h>

#include "enode.h"


/* A general enode constructor. This is not meant to be used directly, but
 * rather via the create_enode_* functions. */

static struct enode *create_enode(int type, 
		struct enode *left,
		struct enode *right,
		float value,
		int function)
{
	struct enode *enode = malloc(sizeof (struct enode));
	if (NULL == enode) {
		perror(NULL);
		exit(EXIT_FAILURE);
	}

	enode->type = type;
	enode->left = left;
	enode->right = right;
	enode->value = value;
	enode->function = function;

	return enode;
}

struct enode *create_enode_constant(float value)
{
	return create_enode(ENODE_CONSTANT, NULL, NULL, value, 0);
}

struct enode *create_enode_op(int type, struct enode *left, struct enode *right)
{
	return create_enode(type, left, right, 0, 0);
}

struct enode *create_enode_not(struct enode *node)
{
	return create_enode(ENODE_NOT, node, NULL, 0, 0);
}

float eval_enode(struct enode *node)
{
	switch (node->type) {
	case ENODE_CONSTANT:
		return node->value;
	case ENODE_GT:
		return eval_enode(node->left) > eval_enode(node->right);
	case ENODE_GTE:
		return eval_enode(node->left) >= eval_enode(node->right);
	case ENODE_LT:
		return eval_enode(node->left) < eval_enode(node->right);
	case ENODE_LTE:
		return eval_enode(node->left) <= eval_enode(node->right);
	case ENODE_EQ:
		return eval_enode(node->left) == eval_enode(node->right);
	case ENODE_NEQ:
		return eval_enode(node->left) != eval_enode(node->right);
	case ENODE_OR:
		return eval_enode(node->left) || eval_enode(node->right);
	case ENODE_AND:
		return eval_enode(node->left) && eval_enode(node->right);
	case ENODE_NOT:
		return ! eval_enode(node->left);
	default:
		fprintf (stderr, "Unknown enode type %d\n", node->type);
		exit(EXIT_FAILURE);
	}
}
