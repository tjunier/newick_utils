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

/* this program is purely lexical */
%option noyywrap

%{
#include <string.h>

int indent_level = 0;
char *indent_string;
void indent(const int, const char*);
char *eol = "\n";

/* we may peform additional operations just after printing a tree */
void cap_tree(char *eol)
{
	if (0 == strcmp(eol, "")) 	/* compact tree */
		printf("\n");	
}

%}

%%

('[^']*')?(:[0-9.]+)?,?	{
	/* possible quoted label, then maybe length */
	indent(indent_level, indent_string);
	printf("%s%s", yytext, eol);
}
([[:graph:]]{-}[(),:']+)?(:[0-9.]+)?,?	{
	/* possible unquoted label, then maybe length */
	indent(indent_level, indent_string);
	printf("%s%s", yytext, eol);
}
"("	{
	indent(indent_level, indent_string);
	printf("(%s", eol); indent_level++;
}
\)('[^']*'|[[:graph:]]{-}[(),:']+)?(:[0-9.]+)?[,;]?	{
	/* possible label (quoted or unquoted), then maybe length. This rule
	 * handles both unquoted or quoted labels - more terse, but less readable...
	 * */
	indent_level--;
	indent(indent_level, indent_string);
	printf("%s%s", yytext, eol);
	if (0 == indent_level) cap_tree(eol);
}
\[[^]]*]	/* ignore comments */ ;
[\t\n ]+ ;

%%

void indent(const int level, const char *indent_string)
{
	int i;
	for (i = 0; i < level; i++)
		printf("%s", indent_string);
}
