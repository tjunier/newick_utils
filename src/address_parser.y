%{
#define YYDEBUG 1
#include <stdio.h>

#include "enode.h"

extern struct enode *expression_root;

void adserror(char *s)
{
	printf ("syntax error in expr (%s).\n", s);
}

%}

%name-prefix="ads"

%union {
	char ival;
	float fval;
	struct enode *enode_p;
}

%token OPEN_PAREN CLOSE_PAREN OP_AND OP_OR OP_NOT

%token <ival> BOOL_FUNC 
%token <fval> CONST
%token <ival> COMPARATOR
%token <ival> NUM_FUNC 

%type <enode_p> comparand
%type <enode_p> comparison
%type <enode_p> term
%type <enode_p> factor

%%

/* I use the words 'term' and 'factor' by analogy with arithmetic operators */

/* TODO: add error-reporting code, so that malformed expressions like 'i
 * l r' are no longer accepted by discarding lookahead tokens. */

expression: /* empty */ { expression_root = NULL; YYACCEPT; }
	| term { expression_root = $1; YYACCEPT; }
	| expression OP_OR term 

term: factor
	| term OP_AND factor {
		struct enode *and = create_enode_op(ENODE_AND, $1, $3);
		$$ = and;
	}

factor: comparison 
      	| BOOL_FUNC {
		struct enode *bf = create_enode_func($1);
		$$ = bf;
	}
	| OP_NOT BOOL_FUNC {
		struct enode *bf = create_enode_func($2);
		struct enode *not = create_enode_not(bf);
		$$ = not;	
	}
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
