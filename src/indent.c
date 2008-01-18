#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

extern char *indent_string;
extern char *eol;

struct parameters {
	int compact;
	};

struct parameters get_params(int argc, char* argv[])
{
	char c;
	struct parameters params;


	indent_string = "  ";
	eol = "\n";
	params.compact = 0;

	while ((c = getopt (argc, argv, "ct:")) != -1) {
		switch (c) {
		case 't':
			 indent_string = optarg;
			 break;
		case 'c':
			 params.compact = 1;
			 break;
		}
	}
	if (params.compact) {
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
		fprintf(stderr, "Usage: %s [-ct:] <filename|->\n", argv[0]);
		exit(EXIT_FAILURE);
        }

	return params;
}


int main(int argc, char* argv[])
{
	struct parameters params = get_params(argc, argv);

	yylex();
}
