#include <stdio.h>
#include <string.h>

#include "svg_graph_radial.h"

unsigned int test_transform_ornaments()
{
	const char *test_name = __func__;

	return 1;
}

int main()
{
	int failures = 0;
	printf("Starting canvas test...\n");
	failures += test_transform_ornaments();
	if (0 == failures) {
		printf("All tests ok.\n");
	} else {
		printf("%d test(s) FAILED.\n", failures);
		return 1;
	}

	return 0;
}
