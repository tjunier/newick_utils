/* 

Copyright (c) 2009 Thomas Junier and Evgeny Zdobnov, University of Geneva
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice,
    this list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright notice,
    this list of conditions and the following disclaimer in the documentation
    and/or other materials provided with the distribution.
* Neither the name of the University of Geneva nor the names of its
    contributors may be used to endorse or promote products derived from this
    software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/
/* A parser for node addresses. Node addresses are used by nw_ed to specify
 * nodes in a tree (analogous to awk or sed). */

%{
#define YYDEBUG 1
#include <stdio.h>

#include "enode.h"
#include "address_parser_status.h"

extern int adslex (void);

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
		if (NULL == or) {
			address_parser_status = ADDRESS_PARSER_MALLOC_ERROR;
			expression_root = NULL;
			YYACCEPT;
		}
		expression_root = or;
		$$ = or;	/* might not be the root */
	}

term: factor
	| term OP_AND factor {
		struct enode *and = create_enode_op(ENODE_AND, $1, $3);
		if (NULL == and) {
			address_parser_status = ADDRESS_PARSER_MALLOC_ERROR;
			expression_root = NULL;
			YYACCEPT;
		}
		$$ = and;
	}

factor: comparison 
      	| BOOL_FUNC {
		struct enode *bf = create_enode_func($1);
		if (NULL == bf) {
			address_parser_status = ADDRESS_PARSER_MALLOC_ERROR;
			expression_root = NULL;
			YYACCEPT;
		}
		$$ = bf;
	}
	| OP_NOT BOOL_FUNC {
		struct enode *bf = create_enode_func($2);
		if (NULL == bf) {
			address_parser_status = ADDRESS_PARSER_MALLOC_ERROR;
			expression_root = NULL;
			YYACCEPT;
		}
		struct enode *not = create_enode_not(bf);
		if (NULL == not) {
			address_parser_status = ADDRESS_PARSER_MALLOC_ERROR;
			expression_root = NULL;
			YYACCEPT;
		}
		$$ = not;	
	}
      	| OPEN_PAREN expression CLOSE_PAREN {
		$$ = $2;
	}
      	| OP_NOT OPEN_PAREN expression CLOSE_PAREN {
		struct enode *not = create_enode_not($3);
		if (NULL == not) {
			address_parser_status = ADDRESS_PARSER_MALLOC_ERROR;
			expression_root = NULL;
			YYACCEPT;
		}
		$$ = not;
	}
	| CONST {
		struct enode *n = create_enode_constant($1);
		if (NULL == n) {
			address_parser_status = ADDRESS_PARSER_MALLOC_ERROR;
			expression_root = NULL;
			YYACCEPT;
		}
		$$ = n;
	}

comparison: comparand COMPARATOR comparand {
		struct enode *c = create_enode_op($2, $1, $3);
		if (NULL == c) {
			address_parser_status = ADDRESS_PARSER_MALLOC_ERROR;
			expression_root = NULL;
			YYACCEPT;
		}
		$$ = c;
	}
	| comparand COMPARATOR comparand COMPARATOR comparand {
		struct enode *c1 = create_enode_op($2, $1, $3);
		if (NULL == c1) {
			address_parser_status = ADDRESS_PARSER_MALLOC_ERROR;
			expression_root = NULL;
			YYACCEPT;
		}
		struct enode *c2 = create_enode_op($4, $3, $5);
		if (NULL == c2) {
			address_parser_status = ADDRESS_PARSER_MALLOC_ERROR;
			expression_root = NULL;
			YYACCEPT;
		}
		struct enode *result = create_enode_op(ENODE_AND, c1, c2);
		if (NULL == result) {
			address_parser_status = ADDRESS_PARSER_MALLOC_ERROR;
			expression_root = NULL;
			YYACCEPT;
		}
		$$ = result;
	}

comparand: CONST {
		struct enode *n = create_enode_constant($1);
		if (NULL == n) {
			address_parser_status = ADDRESS_PARSER_MALLOC_ERROR;
			expression_root = NULL;
			YYACCEPT;
		}
		$$ = n;
	}
	| NUM_FUNC {
		struct enode *f = create_enode_func($1);
		if (NULL == f) {
			address_parser_status = ADDRESS_PARSER_MALLOC_ERROR;
			expression_root = NULL;
			YYACCEPT;
		}
		$$ = f;
	}

%%
