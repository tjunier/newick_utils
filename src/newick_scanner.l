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

char * space2underscore(char * s)
{
	char *p;
	for (p = s; '\0' != *p; p++)
		if (' ' == *p)
			*p = '_';

	return s;
}

%}

%%

'[^']*'	{
	nwslval.sval = (char *) strdup(yytext);
	return LABEL;
 }
[a-zA-Z0-9_.+-]+	{
	nwslval.sval = (char *) strdup(yytext);
	return LABEL;
 }
[a-zA-Z0-9_.+-]+([ ][a-zA-Z0-9_.+-]+)+	{
	nwslval.sval = (char *) space2underscore(strdup(yytext));
	fprintf (stderr, "WARNING: spaces found in label '%s' - converting to underscores.\n",
			yytext);
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
