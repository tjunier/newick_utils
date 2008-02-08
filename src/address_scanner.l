%option prefix = "ads"
%{
#include <string.h>
#include "address_parser.h"

/* Since we use our own prefix 'nws' instead of 'yy', we have to supply
 * nwswrap() (yywrap() was available in libfl) */

int adswrap(void)
{
	return 1;
}

%}

%%

"("	{ return(OPEN_PAREN); }
")"	{ return(CLOSE_PAREN); }
[db]	{
		adslval.cval = yytext[0];
	  	return(NUM_FUNC);
        }
[irl]	{
		adslval.cval = yytext[0];
		return(BOOL_FUNC);
	}
[0-9]+(\.[0-9]+)?	{
		adslval.fval = atof(yytext);
		return(CONST);
	}
[<>] |
"==" |
"!=" |
"<=" |
">="  	{
		adslval.sval = strdup(yytext);
		return(COMPARATOR);
	}
"&"	{ return(OP_AND); }
"|"	{ return(OP_OR); }
"!"	{ return(OP_NOT); }

[\t ]+	/* ignore whitespace */

%%
