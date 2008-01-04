%{
#include <string.h>
#include "newick_parser.h"
%}

%%

'[^']*'	{
	yylval.sval = (char *) strdup(yytext);
	return LABEL;
 }
[a-zA-Z0-9_.-]+	{
	yylval.sval = (char *) strdup(yytext);
	return LABEL;
 }
"("	{ return O_PAREN; }
")"	{ return C_PAREN; }
";"	{ return SEMICOLON; }
","	{ return COMMA; }
":"	{ return COLON; }
\[[^]]*]	/* ignore comments */ ;
[\t\n ]+	/* ignore whitespace */ ;

%%
