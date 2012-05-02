#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "enode.h"
#include "rnode.h"
#include "link.h"
#include "tree_editor_rnode_data.h"

static const float SUPPORT = 2.345;
static const float DEPTH = 3.456;
static const int NB_ANCESTORS = 3;
static const int NB_DESCENDANTS = 7;
static const int NB_CHILDREN = 2;

void setup_current_rnode()
{
	struct rnode *node = create_rnode("any", "");
	struct rnode *kid1 = create_rnode("kid1", "");
	struct rnode *kid2 = create_rnode("kid2", "");
	add_child(node, kid1);
	add_child(node, kid2);
	struct rnode_data *data = malloc(sizeof(struct rnode_data));
	if (NULL == data) { perror(NULL); exit(EXIT_FAILURE); }
	data->nb_ancestors = NB_ANCESTORS;
	data->nb_descendants = NB_DESCENDANTS;
	data->depth = DEPTH;
	data->support = SUPPORT;
	node->data = data;
	enode_eval_set_current_rnode(node);
}

int test_constant()
{
	const char *test_name = "test_constant";

	struct enode * const_num = create_enode_constant(5.5);
	setup_current_rnode();

	if (5.5 != eval_enode(const_num)) {
		printf ("%s: expected 5.5, got %g\n", test_name,
				eval_enode(const_num));
		return 1;
	}

	printf("%s ok.\n", test_name);
	return 0;
}

int test_gt()
{
	const char *test_name = "test_gt";
	
	struct enode * const_1 = create_enode_constant(4.5);
	struct enode * const_2 = create_enode_constant(2);

	setup_current_rnode();

	struct enode * gt = create_enode_op(ENODE_GT, const_1, const_2);
	if (! eval_enode(gt)) {
		printf ("%s: expected (4.5 > 2) == 1\n", test_name);
		return 1;
	}

	gt = create_enode_op(ENODE_GT, const_2, const_1);
	if (eval_enode(gt)) {
		printf ("%s: expected (2 > 4.5) == 0\n", test_name);
		return 1;
	}

	printf("%s ok.\n", test_name);
	return 0;
}

int test_gte()
{
	const char *test_name = "test_gte";
	
	struct enode * const_1 = create_enode_constant(4.5);
	struct enode * const_2 = create_enode_constant(2);
	struct enode * const_3 = create_enode_constant(2.0);
	setup_current_rnode();

	struct enode * gte = create_enode_op(ENODE_GTE, const_1, const_2);
	if (! eval_enode(gte)) {
		printf ("%s: expected (4.5 >= 2) == 1\n", test_name);
		return 1;
	}

	gte = create_enode_op(ENODE_GTE, const_2, const_1);
	if (eval_enode(gte)) {
		printf ("%s: expected (2 >= 4.5) == 0\n", test_name);
		return 1;
	}

	gte = create_enode_op(ENODE_GTE, const_3, const_2);
	if (! eval_enode(gte)) {
		printf ("%s: expected (2.0 >= 2) == 1\n", test_name);
		return 1;
	}

	gte = create_enode_op(ENODE_GTE, const_2, const_3);
	if (! eval_enode(gte)) {
		printf ("%s: expected (2 >= 2.0) == 1\n", test_name);
		return 1;
	}
	printf("%s ok.\n", test_name);
	return 0;
}

int test_lt()
{
	const char *test_name = "test_lt";
	
	struct enode * const_1 = create_enode_constant(4.5);
	struct enode * const_2 = create_enode_constant(2);
	setup_current_rnode();

	struct enode * lt = create_enode_op(ENODE_LT, const_1, const_2);
	if (eval_enode(lt)) {
		printf ("%s: expected (4.5 < 2) == 0\n", test_name);
		return 1;
	}

	lt = create_enode_op(ENODE_LT, const_2, const_1);
	if (!eval_enode(lt)) {
		printf ("%s: expected (2 < 4.5) == 1\n", test_name);
		return 1;
	}

	printf("%s ok.\n", test_name);
	return 0;
}

int test_lte()
{
	const char *test_name = "test_lte";
	
	struct enode * const_1 = create_enode_constant(4.5);
	struct enode * const_2 = create_enode_constant(2);
	struct enode * const_3 = create_enode_constant(2.0);
	setup_current_rnode();

	struct enode * lte = create_enode_op(ENODE_LTE, const_1, const_2);
	if (eval_enode(lte)) {
		printf ("%s: expected (4.5 <= 2) == 0\n", test_name);
		return 1;
	}

	lte = create_enode_op(ENODE_LTE, const_2, const_1);
	if (! eval_enode(lte)) {
		printf ("%s: expected (2 <= 4.5) == 1\n", test_name);
		return 1;
	}

	lte = create_enode_op(ENODE_LTE, const_3, const_2);
	if (! eval_enode(lte)) {
		printf ("%s: expected (2.0 <= 2) == 1\n", test_name);
		return 1;
	}

	lte = create_enode_op(ENODE_LTE, const_2, const_3);
	if (! eval_enode(lte)) {
		printf ("%s: expected (2 <= 2.0) == 1\n", test_name);
		return 1;
	}
	printf("%s ok.\n", test_name);
	return 0;
}

int test_eq()
{
	const char *test_name = "test_eq";
	
	struct enode * const_1 = create_enode_constant(4.5);
	struct enode * const_2 = create_enode_constant(2);
	setup_current_rnode();

	struct enode * eq = create_enode_op(ENODE_EQ, const_1, const_2);
	if (eval_enode(eq)) {
		printf ("%s: expected (4.5 != 2)\n", test_name);
		return 1;
	}

	eq = create_enode_op(ENODE_EQ, const_2, const_2);
	if (!eval_enode(eq)) {
		printf ("%s: expected (2 == 2)\n", test_name);
		return 1;
	}

	printf("%s ok.\n", test_name);
	return 0;
}

int test_neq()
{
	const char *test_name = "test_neq";
	
	struct enode * const_1 = create_enode_constant(4.5);
	struct enode * const_2 = create_enode_constant(2);
	setup_current_rnode();

	struct enode * neq = create_enode_op(ENODE_NEQ, const_1, const_2);
	if (! eval_enode(neq)) {
		printf ("%s: expected (4.5 != 2)\n", test_name);
		return 1;
	}

	neq = create_enode_op(ENODE_NEQ, const_1, const_1);
	if (eval_enode(neq)) {
		printf ("%s: expected (4.5 == 4.5)\n", test_name);
		return 1;
	}

	printf("%s ok.\n", test_name);
	return 0;
}

int test_or()
{
	const char *test_name = "test_or";

	struct enode * const_1 = create_enode_constant(4.5);
	struct enode * const_2 = create_enode_constant(7.8);
	struct enode * gt = create_enode_op(ENODE_GT, const_1, const_2);
	struct enode * lte = create_enode_op(ENODE_LTE, const_1, const_2);
	struct enode * eq = create_enode_op(ENODE_EQ, const_1, const_2);
	struct enode * lt = create_enode_op(ENODE_LT, const_1, const_1);
	setup_current_rnode();

	struct enode * or = create_enode_op(ENODE_OR, gt, lte);
	if (! eval_enode(or)) {
		printf ("%s: got (4.5 > 7.8) || (4.5 <= 7.8) false\n",
				test_name);
		return 1;
	}

	/* test symmetry */
	or = create_enode_op(ENODE_OR, lte, gt);
	if (! eval_enode(or)) {
		printf ("%s: got (4.5 > 7.8) || (4.5 <= 7.8) false\n",
				test_name);
		return 1;
	}

	or = create_enode_op(ENODE_OR, eq, lt);
	if (eval_enode(or)) {
		printf ("%s: got (4.5 == 7.8) || (4.5 < 4.5) true\n",
				test_name);
		return 1;
	}

	printf("%s ok.\n", test_name);
	return 0;
}

int test_and()
{
	const char *test_name = "test_and";

	struct enode * const_1 = create_enode_constant(4.5);
	struct enode * const_2 = create_enode_constant(7.8);
	struct enode * gt = create_enode_op(ENODE_GT, const_1, const_2);
	struct enode * lte = create_enode_op(ENODE_LTE, const_1, const_2);
	struct enode * eq = create_enode_op(ENODE_EQ, const_1, const_1);
	struct enode * lt = create_enode_op(ENODE_LT, const_1, const_2);
	setup_current_rnode();

	/* both false */
	struct enode * and = create_enode_op(ENODE_AND, gt, lte);
	if (eval_enode(and)) {
		printf ("%s: got (4.5 > 7.8) && (4.5 <= 7.8) true\n",
				test_name);
		return 1;
	}

	/* test symmetry */
	and = create_enode_op(ENODE_AND, lte, gt);
	if (eval_enode(and)) {
		printf ("%s: got (4.5 > 7.8) && (4.5 <= 7.8) true\n",
				test_name);
		return 1;
	}

	/* both true */
	and = create_enode_op(ENODE_AND, eq, lt);
	if (! eval_enode(and)) {
		printf ("%s: got (4.5 == 4.5) && (4.5 < 7.8) false\n",
				test_name);
		return 1;
	}

	/* one each */
	and = create_enode_op(ENODE_AND, eq, gt);
	if (eval_enode(and)) {
		printf ("%s: got (4.5 == 4.5) && (4.5 > 7.8) true\n",
				test_name);
		return 1;
	}

	printf("%s ok.\n", test_name);
	return 0;
}

int test_not()
{
	const char *test_name = "test_not";

	struct enode * const_1 = create_enode_constant(4.5);
	struct enode * const_2 = create_enode_constant(7.8);
	struct enode * gt = create_enode_op(ENODE_GT, const_1, const_2);
	struct enode * lte = create_enode_op(ENODE_LTE, const_1, const_2);
	setup_current_rnode();

	/* both false */
	struct enode * and = create_enode_op(ENODE_AND, gt, lte);
	struct enode * not = create_enode_not(and);
	if (! eval_enode(not)) {
		printf ("%s: got ! ((4.5 > 7.8) && (4.5 <= 7.8)) false\n",
				test_name);
		return 1;
	}

	struct enode *notnot = create_enode_not(not);
	if (eval_enode(notnot)) {
		printf ("%s: got !! ((4.5 > 7.8) && (4.5 <= 7.8)) true\n",
				test_name);
		return 1;
	}

	printf("%s ok.\n", test_name);
	return 0;
}

int test_is_leaf()
{
	const char *test_name = "test_is_leaf";

	struct rnode *root = create_rnode("root", "");
	struct rnode *inner = create_rnode("inner", "");
	struct rnode *leaf = create_rnode("leaf", "");

	struct enode *is_leaf = create_enode_func(ENODE_IS_LEAF);

	add_child(root, inner);
	add_child(inner, leaf);

	enode_eval_set_current_rnode(root);
	if (eval_enode(is_leaf)) {
		printf ("%s: function 'f' expected to return false on root\n", test_name);
		return 1;
	}
	enode_eval_set_current_rnode(inner);
	if (eval_enode(is_leaf)) {
		printf ("%s: function 'f' expected to return false on inner node\n", test_name);
		return 1;
	}
	enode_eval_set_current_rnode(leaf);
	if (! eval_enode(is_leaf)) {
		printf ("%s: function 'f' expected to return true on leaf\n", test_name);
		return 1;
	}

	printf("%s ok.\n", test_name);
	return 0;
}

int test_is_inner()
{
	const char *test_name = "test_is_inner";

	struct rnode *root = create_rnode("root", "");
	struct rnode *inner = create_rnode("inner", "");
	struct rnode *leaf = create_rnode("leaf", "");

	struct enode *is_inner_func = create_enode_func(ENODE_IS_INNER);

	add_child(root, inner);
	add_child(inner, leaf);

	enode_eval_set_current_rnode(root);
	if (eval_enode(is_inner_func)) {
		printf ("%s: function 'i' expected to return false on root\n", test_name);
		return 1;
	}
	enode_eval_set_current_rnode(inner);
	if (! eval_enode(is_inner_func)) {
		printf ("%s: function 'i' expected to return true on inner node\n", test_name);
		return 1;
	}
	enode_eval_set_current_rnode(leaf);
	if (eval_enode(is_inner_func)) {
		printf ("%s: function 'i' expected to return false on leaf\n", test_name);
		return 1;
	}

	printf("%s ok.\n", test_name);
	return 0;
}

int test_is_root()
{
	const char *test_name = "test_is_root";

	struct rnode *root = create_rnode("root", "");
	struct rnode *inner = create_rnode("inner", "");
	struct rnode *leaf = create_rnode("leaf", "");

	struct enode *is_root_func = create_enode_func(ENODE_IS_ROOT);

	add_child(root, inner);
	add_child(inner, leaf);

	enode_eval_set_current_rnode(root);
	if (!eval_enode(is_root_func)) {
		printf ("%s: function 'r' expected to return true on root\n", test_name);
		return 1;
	}
	enode_eval_set_current_rnode(inner);
	if (eval_enode(is_root_func)) {
		printf ("%s: function 'r' expected to return false on inner node\n", test_name);
		return 1;
	}
	enode_eval_set_current_rnode(leaf);
	if (eval_enode(is_root_func)) {
		printf ("%s: function 'r' expected to return false on leaf\n", test_name);
		return 1;
	}

	printf("%s ok.\n", test_name);
	return 0;
}

int test_support()
{
	const char *test_name = "test_support";

	struct enode *expr = create_enode_func(ENODE_SUPPORT);
	setup_current_rnode();

	if (eval_enode(expr) != SUPPORT) {
		printf ("%s: expected support value of %f, got %f.\n",
				test_name, SUPPORT, eval_enode(expr));
		return 1;
	}

	printf("%s ok.\n", test_name);
	return 0;
}

int test_depth()
{
	const char *test_name = "test_depth";

	struct enode *expr = create_enode_func(ENODE_DEPTH);
	setup_current_rnode();

	if (eval_enode(expr) != DEPTH) {
		printf ("%s: expected depth value of %g, got %g.\n",
				test_name, DEPTH, eval_enode(expr));
		return 1;
	}

	printf("%s ok.\n", test_name);
	return 0;
}

int test_nb_ancestors()
{
	const char *test_name = "test_nb_ancestors";

	struct enode *expr = create_enode_func(ENODE_NB_ANCESTORS);
	setup_current_rnode();

	if (eval_enode(expr) != NB_ANCESTORS) {
		printf ("%s: expected %d ancestors, got %g.\n",
				test_name, NB_ANCESTORS, eval_enode(expr));
		return 1;
	}

	printf("%s ok.\n", test_name);
	return 0;
}

int test_nb_descendants()
{
	const char *test_name = "test_nb_descendants";

	struct enode *expr = create_enode_func(ENODE_NB_DESCENDANTS);
	setup_current_rnode();

	if (eval_enode(expr) != NB_DESCENDANTS) {
		printf ("%s: expected %d descendants, got %g.\n",
				test_name, NB_CHILDREN, eval_enode(expr));
		return 1;
	}

	printf("%s ok.\n", test_name);
	return 0;
}

int test_nb_children()
{
	const char *test_name = "test_nb_children";

	struct enode *expr = create_enode_func(ENODE_NB_CHILDREN);
	setup_current_rnode();

	if (eval_enode(expr) != NB_CHILDREN) {
		printf ("%s: expected %d descendants, got %g.\n",
				test_name, NB_DESCENDANTS, eval_enode(expr));
		return 1;
	}

	printf("%s ok.\n", test_name);
	return 0;
}

int main()
{
	int failures = 0;
	printf("Starting enode test...\n");
	failures += test_constant();
	failures += test_gt();
	failures += test_gte();
	failures += test_lt();
	failures += test_lte();
	failures += test_eq();
	failures += test_neq();
	failures += test_or();
	failures += test_and();
	failures += test_not();
	failures += test_is_root();
	failures += test_is_inner();
	failures += test_is_leaf();
	failures += test_support();
	failures += test_depth();
	failures += test_nb_ancestors();
	failures += test_nb_descendants();
	failures += test_nb_children();
	if (0 == failures) {
		printf("All tests ok.\n");
	} else {
		printf("%d test(s) FAILED.\n", failures);
		return 1;
	}

	return 0;
}
