%{
#include <stdio.h>

void adserror(char *s)
{
	printf ("syntax error in expr.\n");
}

%}

%name-prefix="ads"

%union {
	char cval;
	char *sval;
	float fval;
}

%token OPEN_PAREN CLOSE_PAREN NUM_FUNC COMPARATOR BOOL_FUNC OP_AND OP_OR OP_NOT
%token <fval> CONST

%type <fval> comparand

%%

/* I use the words 'term' and 'factor' by analogy with arithmetic operators */

expression: term
	  | expression OP_OR term
	  | OP_NOT expression

term: factor
    | term OP_AND factor

factor: comparison
      | BOOL_FUNC
      | OPEN_PAREN expression CLOSE_PAREN

comparison: comparand COMPARATOR comparand

comparand: CONST {
	 	printf ("const: %f\n", $1); 
	 }
	 | NUM_FUNC

%%
