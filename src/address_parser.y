%{
#include <stdio.h>

void adserror(char *s)
{
	printf ("syntax error in expr.\n");
}

%}

%name-prefix="ads"
%token OPEN_PAREN CLOSE_PAREN CONST NUM_FUNC COMPARATOR BOOL_FUNC OP_AND OP_OR OP_NOT

%%

/* I use the words 'term' and 'factor' by analogy with arithmetic operators */

expression: term
	  | expression OP_OR term
	  | OP_NOT expression

term: factor
    | term OP_AND factor

factor: comparison
      | BOOL_FUNC

comparison: comparand COMPARATOR comparand

comparand: CONST
	 | NUM_FUNC

%%
