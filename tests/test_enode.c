#include <stdio.h>
#include <string.h>

#include "enode.h"

int test_constant()
{
	const char *test_name = "test_constant";

	struct enode * const_num = create_enode_constant(5.5);

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
int main()
{
	int failures = 0;
	printf("Starting canvas test...\n");
	failures += test_constant();
	failures += test_gt();
	failures += test_gte();
	failures += test_lt();
	failures += test_lte();
	if (0 == failures) {
		printf("All tests ok.\n");
	} else {
		printf("%d test(s) FAILED.\n", failures);
		return 1;
	}

	return 0;
}
