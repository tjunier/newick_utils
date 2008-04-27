%option prefix = "nws"
%{
#include <string.h>
#include "newick_parser.h"

/* Since we use our own prefix 'nws' instead of 'yy', we have to supply
 * nwswrap() (yywrap() was available in libfl) */

int nwswrap(void)
{
	return 1;
}

int lineno = 0;

%}

%%

'[^']*'	{
	nwslval.sval = (char *) strdup(yytext);
	return LABEL;
 }
[a-zA-Z0-9_.-]+	{
	nwslval.sval = (char *) strdup(yytext);
	return LABEL;
 }
"("	{ return O_PAREN; }
")"	{ return C_PAREN; }
";"	{ return SEMICOLON; }
","	{ return COMMA; }
":"	{ return COLON; }
\[[^]]*]	/* ignore comments */ ;
[\t ]+	/* ignore whitespace */ ;
\n 	{ lineno++; }

%%
