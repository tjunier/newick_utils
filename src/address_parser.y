%{
#include <stdio.h>

#include "enode.h"

extern struct enode *expression_root;

void adserror(char *s)
{
	printf ("syntax error in expr.\n");
}

%}

%name-prefix="ads"

%union {
	char cval;
	char ival;
	float fval;
	struct enode *enode_p;
}

%token OPEN_PAREN CLOSE_PAREN BOOL_FUNC OP_AND OP_OR OP_NOT
%token <fval> CONST
%token <ival> COMPARATOR
%token <cval> NUM_FUNC 

%type <enode_p> comparand
%type <enode_p> comparison
%type <enode_p> term
%type <enode_p> factor

%%

/* I use the words 'term' and 'factor' by analogy with arithmetic operators */

expression: term { expression_root = $1; YYACCEPT; }
	| expression OP_OR term

term: factor
	| term OP_AND factor

factor: comparison 
      	| BOOL_FUNC
	| OP_NOT BOOL_FUNC
      	| OPEN_PAREN expression CLOSE_PAREN
      	| OP_NOT OPEN_PAREN expression CLOSE_PAREN

comparison: comparand COMPARATOR comparand {
		struct enode *c = create_enode_op($2, $1, $3);
		// printf ("%s - comparison\n", $2);
		$$ = c;
	}

comparand: CONST {
	 	// printf ("const: %f\n", $1); 
		struct enode *n = create_enode_constant($1);
		$$ = n;
	}
	| NUM_FUNC {
		printf ("nfunc: %c\n", $1);
	}

%%
