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
[db]	{ return(NUM_FUNC); }
[irl]	{ return(BOOL_FUNC); }
[0-9]+(\.[0-9]+)?	{ return(CONST); }
[<>] |
"==" |
"!=" |
"<=" |
">="  	{ return(COMPARATOR); }
"&"	{ return(OP_AND); }
"|"	{ return(OP_OR); }
"!"	{ return(OP_NOT); }

[\t ]+	/* ignore whitespace */

%%
