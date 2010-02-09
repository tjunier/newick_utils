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
"c"	{ adslval.ival = ENODE_NB_CHILDREN; return(NUM_FUNC); }
"D"	{ adslval.ival = ENODE_NB_DESCENDANTS; return(NUM_FUNC); }

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
