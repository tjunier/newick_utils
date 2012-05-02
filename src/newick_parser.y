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
%{
#include <stdio.h>
#include "rnode.h"
#include "list.h"
#include "link.h"
#include "parser.h"

/* in the (admittedly artificial) case of trees with lots of nesting on the
 * left side, the stack can be exhausted. I set this to 100,000, which is
 * enough for a 100,000-leaf, completely unbalanced left-leaning tree. */

/* NOTE: this grammar generates 2 shift/reduce and 1 reduce/reduce conflicts,
 * but they are all due to the error-reporting code. For a correct tree, the
 * grammar is unambiguous. According to the test suite, Bison resolves the
 * conflicts correctly anyway. */

#define YYMAXDEPTH 100000

extern struct llist *nodes_in_order;
extern struct rnode *root;
extern enum parser_status_type newick_parser_status;

extern int lineno;
char *nwsget_text();
extern int nwslex (void);

void nwserror(char *s)
{
	s = s;	/* suppresses warning about unused s */
	printf ("ERROR: Syntax error at line %d near '%s'\n",
		lineno, nwsget_text());
}

%}

/* %error-verbose */

%name-prefix="nws"

%union {
	char *sval;
	struct rnode *nodep;
	struct llist *llistp;
}

%token O_PAREN COLON COMMA C_PAREN SEMICOLON
%token <sval> LABEL
%token TOK_EOF	0

%type <nodep> leaf
%type <nodep> node
%type <llistp> nodelist
%type <nodep> inner_node

%%

tree: /* empty */	{
		root = NULL;
		newick_parser_status = PARSER_STATUS_EMPTY;
		YYACCEPT;
	}
    | node SEMICOLON {
    		root = $1;
		YYACCEPT;
    }

    /* This warning produces a reduce/reduce conflict. Bison solves it
 * correctly, according to the test suite. */

    | node { 	
	fprintf (stderr, "ERROR: missing ';' at end of tree, line %d "
		"near '%s'\n", lineno, nwsget_text());
	root = NULL;
	newick_parser_status = PARSER_STATUS_PARSE_ERROR;
	YYACCEPT;
    }
    ;

node: 	leaf {
		if (! append_element(nodes_in_order, $1)) {
			root = NULL;
			newick_parser_status = PARSER_STATUS_MALLOC_ERROR;
			YYACCEPT;
		}
	}
    	| inner_node {
    		if (! append_element(nodes_in_order, $1)) {
			root = NULL;
			newick_parser_status = PARSER_STATUS_MALLOC_ERROR;
			YYACCEPT;
		}
	}
    ;

inner_node: O_PAREN nodelist C_PAREN {
		struct list_elem* lep;
		struct rnode *np;
		np = create_rnode("","");
		if (NULL == np) {
			newick_parser_status = PARSER_STATUS_MALLOC_ERROR;
			root = NULL;
			YYACCEPT;
		}
		for (lep = $2->head; NULL != lep; lep = lep->next)
			add_child(np, (struct rnode*) lep->data);
		destroy_llist($2);
		$$ = np;
    }
    | O_PAREN nodelist C_PAREN LABEL {
		struct list_elem* lep;
		struct rnode *np;
		np = create_rnode($4,"");
		if (NULL == np) {
			newick_parser_status = PARSER_STATUS_MALLOC_ERROR;
			root = NULL;
			YYACCEPT;
		}
		free($4);
		for (lep = $2->head; NULL != lep; lep = lep->next) 
			add_child(np, (struct rnode*) lep->data);
		destroy_llist($2);
		$$ = np;
    }
    | O_PAREN nodelist C_PAREN LABEL COLON LABEL {
		struct list_elem* lep;
		struct rnode *np;
		np = create_rnode($4,$6);
		if (NULL == np) {
			newick_parser_status = PARSER_STATUS_MALLOC_ERROR;
			root = NULL;
			YYACCEPT;
		}
		free($4);
		for (lep = $2->head; NULL != lep; lep = lep->next) 
			add_child(np, (struct rnode*) lep->data); 
		destroy_llist($2);
		free($6);
		$$ = np;
    }	
    | O_PAREN nodelist C_PAREN COLON LABEL {
		struct list_elem* lep;
		struct rnode *np;
		np = create_rnode("",$5);
		if (NULL == np) {
			newick_parser_status = PARSER_STATUS_MALLOC_ERROR;
			root = NULL;
			YYACCEPT;
		}
		for (lep = $2->head; NULL != lep; lep = lep->next) 
			add_child(np, (struct rnode*) lep->data); 
		destroy_llist($2);
		free($5);
		$$ = np;
    }
    | O_PAREN nodelist { 
	fprintf (stderr, "ERROR: missing ')' at line %d near '%s'\n",
		lineno, nwsget_text());
	root = NULL;
	YYACCEPT;	
    }
    ;

nodelist: node {
		struct llist *listp;
		listp = create_llist();
		if (NULL == listp) {
			newick_parser_status = PARSER_STATUS_MALLOC_ERROR;
			root = NULL;
			YYACCEPT;
		}
		if (! append_element(listp, $1)) {
			newick_parser_status = PARSER_STATUS_MALLOC_ERROR;
			root = NULL;
			YYACCEPT;
		}
			
		$$ = listp;
	}
	| nodelist COMMA node {
		struct llist *listp = $1;
		if (! append_element(listp, $3)) {
			newick_parser_status = PARSER_STATUS_MALLOC_ERROR;
			root = NULL;
			YYACCEPT;
		}
		$$ = listp;
	}
	;

leaf: LABEL {
		struct rnode *np;
		np = create_rnode($1,"");
		if (NULL == np) {
			newick_parser_status = PARSER_STATUS_MALLOC_ERROR;
			root = NULL;
			YYACCEPT;
		}
		free($1);
		$$ = np;
	}
    | LABEL COLON LABEL {
		struct rnode *np;
		np = create_rnode($1,$3);
		if (NULL == np) {
			newick_parser_status = PARSER_STATUS_MALLOC_ERROR;
			root = NULL;
			YYACCEPT;
		}
		free($1);
		free($3);
		$$ = np;
	}
    | COLON LABEL {
		struct rnode *np = create_rnode("",$2);
		if (NULL == np) {
			newick_parser_status = PARSER_STATUS_MALLOC_ERROR;
			root = NULL;
			YYACCEPT;
		}
		free($2);
		$$ = np;
	}
    | /* empty */ {
		struct rnode *np = create_rnode("","");
		if (NULL == np) {
			newick_parser_status = PARSER_STATUS_MALLOC_ERROR;
			root = NULL;
			YYACCEPT;
		}
		$$ = np;
	}
    ;

%%
