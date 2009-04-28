#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "common.h"

extern char *indent_string;
extern char *eol;

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
	int compact = FALSE;

	eol = "\n";
	indent_string = "  ";

	while ((c = getopt (argc, argv, "cht:")) != -1) {
		switch (c) {
		case 'c':
			 compact = TRUE;
			 break;
		case 'h':
			help(argv);
			exit(EXIT_SUCCESS);
		case 't':
			 indent_string = optarg;
			 break;
		}
	}
	if (TRUE == compact) {
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
