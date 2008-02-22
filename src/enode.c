#include <stdlib.h>
#include <stdio.h>

#include "enode.h"
#include "rnode.h"
#include "tree_editor_rnode_data.h"

/* Functions that operate on a tree node (e.g., to return whether or not it is
 * a leaf, or its depth in the tree, etc) operate on this external variable. It
 * is static on purpose, so that it stays "private". To set it, use
 * enode_eval_set_current_rnode(). */

static struct rnode *current_tree_node;

/* A general enode constructor. This is not meant to be used directly, but
 * rather via the create_enode_* functions. */

static struct enode *create_enode(int type, 
		struct enode *left,
		struct enode *right,
		float value)
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

	return enode;
}

struct enode *create_enode_constant(float value)
{
	return create_enode(ENODE_CONSTANT, NULL, NULL, value);
}

struct enode *create_enode_op(int type, struct enode *left, struct enode *right)
{
	return create_enode(type, left, right, 0);
}

struct enode *create_enode_not(struct enode *node)
{
	return create_enode(ENODE_NOT, node, NULL, 0);
}

struct enode *create_enode_func(int type)
{
	return create_enode(type, NULL, NULL, 0);
}

void enode_eval_set_current_rnode(struct rnode *tree_node)
{
	current_tree_node = tree_node;
}

/* Design note: evaluating an enode sometimes requires calling a function on an
 * rnode (tree node). There are two ways of doing this (at least): pass the
 * rnode as an argument to eval_enode(), or use "global" variable. The first
 * style would involve carrying around an argument that is not always required
 * (for all enodes that do not depend on an rnode), but the second style is
 * somewhat dangerous becaue globals are ingerently dangerous. I chose the
 * second style, but making the variable static and using a function to set it:
 * this should avoid unwanted write accesses. */

float eval_enode(struct enode *node)
{
	struct rnode_data *data;
	data = (struct rnode_data *) current_tree_node->data;

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
	case ENODE_IS_INNER:
		return is_inner_node(current_tree_node);
	case ENODE_IS_ROOT:
		return is_root(current_tree_node);
	case ENODE_IS_LEAF:
		return is_leaf(current_tree_node);
	case ENODE_SUPPORT:
		return data->support;
	case ENODE_DEPTH:
		return data->depth;
	case ENODE_NB_ANCESTORS:
		return data->nb_ancestors;
	default:
		fprintf (stderr, "Unknown enode type %d\n", node->type);
		exit(EXIT_FAILURE);
	}
}
