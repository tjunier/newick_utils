#include <stdio.h>

#include "enode.h"

extern int adsdebug;

struct enode *expression_root;

int main()
{
	// adsdebug = 1;

	adsparse();
	// printf ("expr head: %p\n", expression_root);
	printf ("%g\n", eval_enode(expression_root));

	return 0;
}
