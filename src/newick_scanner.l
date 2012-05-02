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
%option prefix="nws"
%{
#include <string.h>
#include "newick_parser.h"

/* I'd have liked to #include a header file with those definitions, but Bison
 * puts it into a .c file (newick_parser.c). So I have top copy it literally. */

#ifndef YY_BUF_SIZE
#define YY_BUF_SIZE 16384
#endif

/* Since we use our own prefix 'nws' instead of 'yy', we have to supply
 * nwswrap() (yywrap() was available in libfl) */

int nwswrap(void)
{
	return 1;
}

int lineno = 0;

/* ! modifies its argument */

char * space2underscore(char * s)
{
	char *p;
	for (p = s; '\0' != *p; p++)
		if (' ' == *p)
			*p = '_';

	return s;
}

YY_BUFFER_STATE string_buf_state;
YY_BUFFER_STATE file_buf_state;

/* The following two functions are used when lexing from a string. */

void newick_scanner_set_string_input(char *input)
{
	string_buf_state = yy_scan_string(input);
}

void newick_scanner_clear_string_input()
{
	yy_delete_buffer(string_buf_state);
}

/* And these are for switching to a FILE */

void newick_scanner_set_file_input(FILE *input)
{
	file_buf_state = yy_create_buffer(input, YY_BUF_SIZE);
	yy_switch_to_buffer(file_buf_state);
}

%}

/* See http://evolution.genetics.washington.edu/phylip/newick_doc.html for the
 * grammar of Newick. Do info flex for the regexp syntax. */ 

%%

('[^']*')+	{
	/* quoted string (one or more) */
	nwslval.sval = (char *) strdup(yytext);
	return LABEL;
 }
 /* NOTE: it seems that some (older?) versions of Flex don't recognize the '{-}'
  * (set difference) operator. For now, I don't attempt to support these. */
[[:graph:]]{-}[();,:'\[\]]+	{
	/* printable characters except ();,:'[] */
	nwslval.sval = (char *) strdup(yytext);
	return LABEL;
 }
[[:graph:]]{-}[();,:'\[\]]+(" "+[[:graph:]]{-}[();,:'\[\]]+)+ {
	/* the same, with possible spaces in the middle (technically not
	 * Newick, but can be fixed */
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
