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
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "common.h"

extern char *indent_string;
extern char *eol;
extern int yylex(void);

void help(char *argv[])
{
	printf (
"Indents the Newick, making structure more clear.\n"
"\n"
"Synopsis\n"
"--------\n"
"\n"
"%s [-cht:] <newick trees filename|->\n"
"\n"
"Input\n"
"-----\n"
"\n"
"Argument is the name of a file that contains Newick trees, or '-' (in\n"
"which case trees are read from standard input).\n"
"\n"
"Output\n"
"------\n"
"\n"
"By default, prints the input tree, with each parenthesis and each leaf on a\n"
"line of its own, and indented a multiple of '  ' (two spaces) to reflect\n"
"structure. The default output is valid Newick.\n"
"\n"
"This is useful for seeing the structure of larger trees, and also for\n"
"editing them. \n"
"\n"
"Indenting is purely lexical, therefore it tolerates syntax errors. A tree\n"
"that does not parse can be indented for examination, corrected, then\n"
"compacted (see Options).\n"
"\n"
"Options\n"
"-------\n"
"\n"
"    -c: compact - remove indentation \n"
"    -h: print this message and exit\n"
"    -t <text>: use 'text' instead of the default '  '. WARNING: using\n"
"       anything else than whitespace is likely to result in output that\n"
"       is not valid Newick (which may or may not be a problem).\n"
"       Ignored with '-c'.\n"
"\n"
"Examples\n"
"--------\n"
"\n"
"# Indent\n"
"$ %s data/catarrhini\n"
"\n"
"# Indent, with visual guides for indentation (NOT valid Newick anymore)\n"
"$ %s -t '|  ' data/catarrhini\n",
	argv[0],
	argv[0],
	argv[0]
	);
}

void get_params(int argc, char* argv[])
{
	int c;
	bool compact = false;

	eol = "\n";
	indent_string = "  ";

	while ((c = getopt (argc, argv, "cht:")) != -1) {
		switch (c) {
		case 'c':
			 compact = true;
			 break;
		case 'h':
			help(argv);
			exit(EXIT_SUCCESS);
		case 't':
			 indent_string = optarg;
			 break;
		}
	}
	if (true == compact) {
		eol = "";
		indent_string = "";
	}
	if (1 == (argc - optind))       {
		if (0 != strcmp("-", argv[optind])) {
			FILE *fin = fopen(argv[optind], "r");
			extern FILE *yyin;
			if (NULL == fin) {
				perror(NULL);
				exit(EXIT_FAILURE);
			}
			yyin = fin;
		}
	} else {
		fprintf(stderr, "Usage: %s [-cht:] <filename|->\n", argv[0]);
		exit(EXIT_FAILURE);
        }
}


int main(int argc, char* argv[])
{
	get_params(argc, argv);

	yylex();

	return 0;
}
