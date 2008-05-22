/* The node address lexer (see address_scanner.y) */

%option prefix = "ads"
%{
#include <string.h>
#include "enode.h"
#include "address_parser.h"

/* Since we use our own prefix 'ads' instead of 'yy', we have to supply
 * ads() (yywrap() was available in libfl) */

int adswrap(void)
{
	return 1;
}

YY_BUFFER_STATE buf_state;

void address_scanner_set_input(char *input)
{
	buf_state = yy_scan_string(input);
}

void address_scanner_clear_input()
{
	yy_delete_buffer(buf_state);
}

%}

%%

"("	{ return(OPEN_PAREN); }
")"	{ return(CLOSE_PAREN); }

"d"	{ adslval.ival = ENODE_DEPTH; return(NUM_FUNC); }
"b"	{ adslval.ival = ENODE_SUPPORT; return(NUM_FUNC); }
"a"	{ adslval.ival = ENODE_NB_ANCESTORS; return(NUM_FUNC); }

"i"	{ adslval.ival = ENODE_IS_INNER; return(BOOL_FUNC); }
"r"	{ adslval.ival = ENODE_IS_ROOT; return(BOOL_FUNC); }
"l"	{ adslval.ival = ENODE_IS_LEAF; return(BOOL_FUNC); }

[0-9]+(\.[0-9]+)?	{
		adslval.fval = atof(yytext);
		return(CONST);
	}

">"	{ adslval.ival = ENODE_GT; return(COMPARATOR); }
">="	{ adslval.ival = ENODE_GTE; return(COMPARATOR); }
"<"	{ adslval.ival = ENODE_LT; return(COMPARATOR); }
"<="	{ adslval.ival = ENODE_LTE; return(COMPARATOR); }
"==" 	{ adslval.ival = ENODE_EQ; return(COMPARATOR); }
"!=" 	{ adslval.ival = ENODE_NEQ; return(COMPARATOR); }

"&"	{ return(OP_AND); }
"|"	{ return(OP_OR); }
"!"	{ return(OP_NOT); }

[\t \n]+	/* ignore whitespace */

%%
