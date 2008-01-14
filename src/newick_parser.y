%{
#include <stdio.h>
/*#include "newick.tab.h"*/ /* Does not compile on MacOSX with this in */
#include "rnode.h"
#include "redge.h"
#include "list.h"
#include "to_newick.h"

/* in the (admittedly artificial) case of trees with lots of nesting on the
 * left side, the stack can be exhausted. I set this to 100,000, which is
 * enough for a 100,000-leaf, completely unbalanced left-leaning tree. */

#define YYMAXDEPTH 100000

void yyerror(char *s)
{
	printf ("goof!\n");
}

extern struct llist *nodes_in_order;
extern struct rnode *root;

%}

/* %error-verbose */

%union {
	char *sval;
	struct redge *edgep;
	struct llist *llistp;
}

%token O_PAREN COLON COMMA C_PAREN SEMICOLON
%token <sval> LABEL
%token TOK_EOF	0

%type <edgep> leaf
%type <edgep> node
%type <llistp> nodelist
%type <edgep> inner_node

%%

tree: /* empty */	{ root = NULL; YYACCEPT; }
    | node SEMICOLON {
    		root = (struct rnode *)$1->child_node; 
		YYACCEPT;
    }
    ;

node: leaf { append_element(nodes_in_order, (struct rnode*) $1->child_node); }
    | inner_node { append_element(nodes_in_order, (struct rnode*) $1->child_node); }
    ;

inner_node: O_PAREN nodelist C_PAREN {
		struct list_elem* lep;
		struct rnode *np;
		struct redge *ep;
		np = create_rnode("");
		for (lep = $2->head; NULL != lep; lep = lep->next) {
			add_child_edge(np, (struct redge*) lep->data);
		}
		destroy_llist($2);
		ep = create_redge(NULL);
		set_parent_edge(np, ep);
		$$ = ep;
    }
    | O_PAREN nodelist C_PAREN LABEL {
		struct list_elem* lep;
		struct rnode *np;
		struct redge *ep;
		np = create_rnode($4);
		free($4);
		for (lep = $2->head; NULL != lep; lep = lep->next) {
			add_child_edge(np, (struct redge*) lep->data);
		}
		destroy_llist($2);
		ep = create_redge(NULL);
		set_parent_edge(np, ep);
		$$ = ep;
    }
    | O_PAREN nodelist C_PAREN LABEL COLON LABEL {
		struct list_elem* lep;
		struct rnode *np;
		struct redge *ep;
		np = create_rnode($4);
		free($4);
		for (lep = $2->head; NULL != lep; lep = lep->next) {
			add_child_edge(np, (struct redge*) lep->data);
		}
		destroy_llist($2);
		ep = create_redge($6);
		set_parent_edge(np, ep);
		free($6);
		$$ = ep;
    }	
    | O_PAREN nodelist C_PAREN COLON LABEL {
		struct list_elem* lep;
		struct rnode *np;
		struct redge *ep;
		np = create_rnode("");
		for (lep = $2->head; NULL != lep; lep = lep->next) {
			add_child_edge(np, (struct redge*) lep->data);
		}
		destroy_llist($2);
		ep = create_redge($5);
		set_parent_edge(np, ep);
		free($5);
		$$ = ep;
    }
    ;

nodelist: node {
		struct llist *listp;
		listp = create_llist();
		append_element(listp, $1);
		$$ = listp;
	}
	| nodelist COMMA node {
		struct llist *listp = $1;
		append_element(listp, $3);
		$$ = listp;
	}
	;

leaf: LABEL {
		struct redge *ep;
		struct rnode *np;
		ep = create_redge(NULL);
		np = create_rnode($1);
		free($1);
		set_parent_edge(np, ep);
		$$ = ep;
	}
    | LABEL COLON LABEL {
		struct redge *ep;
		struct rnode *np;
		ep = create_redge($3);
		free($3);
		np = create_rnode($1);
		free($1);
		set_parent_edge(np, ep);
		$$ = ep;
	}
    ;

%%
