/* A parser for node addresses. Node addresses are used by nw_ed to specify
 * nodes in a tree (analogous to awk or sed). */

%{
#define YYDEBUG 1
#include <stdio.h>

#include "enode.h"

/* The root of the expression (when represented as a parse tree) */
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
%type <enode_p> expression

%%

/* I use the words 'term' and 'factor' by analogy with arithmetic operators */

expression: /* empty */ { expression_root = NULL; }
	| term { expression_root = $1; }
	| expression OP_OR term {
		struct enode *or = create_enode_op(ENODE_OR, $1, $3);
		expression_root = or;
		$$ = or;	/* might not be the root */
	}

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
      	| OPEN_PAREN expression CLOSE_PAREN {
		$$ = $2;
	}
      	| OP_NOT OPEN_PAREN expression CLOSE_PAREN {
		struct enode *not = create_enode_not($3);
		$$ = not;
	}

comparison: comparand COMPARATOR comparand {
		struct enode *c = create_enode_op($2, $1, $3);
		$$ = c;
	}
	| comparand COMPARATOR comparand COMPARATOR comparand {
		struct enode *c1 = create_enode_op($2, $1, $3);
		struct enode *c2 = create_enode_op($4, $3, $5);
		struct enode *res = create_enode_op(ENODE_AND, c1, c2);
		$$ = res;
	}

comparand: CONST {
		struct enode *n = create_enode_constant($1);
		$$ = n;
	}
	| NUM_FUNC {
		struct enode *f = create_enode_func($1);
		$$ = f;
	}

%%
