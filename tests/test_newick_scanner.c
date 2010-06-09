#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h> 

#include "rnode.h"
#include "list.h"
#include "parser.h"
#include "newick_parser.h"

extern FILE *nwsin;

int nwslex (void);
struct rnode *root;
struct llist *nodes_in_order;
enum parser_status_type newick_parser_status;

int test_simple()
{
	const char *test_name = "test_simple";

	/* Note: this is a valid Newick string, but since we're checking the
	 * scanner, it doesn't have to be - see test_garbled()  */
	char *newick = "((A,B),C);";
	newick_scanner_set_string_input(newick);

	int token_type = -1;

	token_type = nwslex();	/* ( */
	if (O_PAREN != token_type) {
		printf ("%s: expected token type %d, got %d\n",
				test_name, O_PAREN, token_type);
		return 1;
	}
	token_type = nwslex(); 	/* ( */
	if (O_PAREN != token_type) {
		printf ("%s: expected token type %d, got %d\n",
				test_name, O_PAREN, token_type);
		return 1;
	}
	token_type = nwslex();	/* A */
	if (LABEL != token_type) {
		printf ("%s: expected token type %d, got %d\n",
				test_name, LABEL, token_type);
		return 1;
	}
	if (strcmp(nwslval.sval, "A") != 0) {
		printf("%s: expected label 'A', got '%s'\n",
				test_name, nwslval.sval);
		return 1;
	}
	token_type = nwslex();	/* , */
	if (COMMA != token_type) {
		printf ("%s: expected token type %d, got %d\n",
				test_name, COMMA, token_type);
		return 1;
	}
	token_type = nwslex();	/* B */
	if (LABEL != token_type) {
		printf ("%s: expected token type %d, got %d\n",
				test_name, LABEL, token_type);
		return 1;
	}
	if (strcmp(nwslval.sval, "B") != 0) {
		printf("%s: expected label 'B', got '%s'\n",
				test_name, nwslval.sval);
		return 1;
	}
	token_type = nwslex();	/* ) */
	if (C_PAREN != token_type) {
		printf ("%s: expected token type %d, got %d\n",
				test_name, C_PAREN, token_type);
		return 1;
	}
	token_type = nwslex();	/* , */
	if (COMMA != token_type) {
		printf ("%s: expected token type %d, got %d\n",
				test_name, COMMA, token_type);
		return 1;
	}
	token_type = nwslex();	/* C */
	if (LABEL != token_type) {
		printf ("%s: expected token type %d, got %d\n",
				test_name, LABEL, token_type);
		return 1;
	}
	if (strcmp(nwslval.sval, "C") != 0) {
		printf("%s: expected label 'C', got '%s'\n",
				test_name, nwslval.sval);
		return 1;
	}
	token_type = nwslex();	/* ) */
	if (C_PAREN != token_type) {
		printf ("%s: expected token type %d, got %d\n",
				test_name, C_PAREN, token_type);
		return 1;
	}
	token_type = nwslex();	/* ; */
	if (SEMICOLON != token_type) {
		printf ("%s: expected token type %d, got %d\n",
				test_name, SEMICOLON, token_type);
		return 1;
	}
	token_type = nwslex();	/* EOF */
	if (token_type > 0) {
		printf ("%s: expected EOF, got %d\n",
				test_name, EOF, token_type);
		return 1;
	}

	printf("%s ok.\n", test_name);
	return 0;
}

int test_garbled()
{
	const char *test_name = "test_garbled";

	char *newick = ")ABC(;(,";
	newick_scanner_set_string_input(newick);

	int token_type = -1;

	token_type = nwslex();
	if (C_PAREN != token_type) {
		printf ("%s: expected token type %d, got %d\n",
				test_name, C_PAREN, token_type);
		return 1;
	}
	token_type = nwslex();
	if (LABEL != token_type) {
		printf ("%s: expected token type %d, got %d\n",
				test_name, LABEL, token_type);
		return 1;
	}
	if (strcmp(nwslval.sval, "ABC") != 0) {
		printf("%s: expected label 'ABC', got '%s'\n",
				test_name, nwslval.sval);
		return 1;
	}
	token_type = nwslex();
	if (O_PAREN != token_type) {
		printf ("%s: expected token type %d, got %d\n",
				test_name, O_PAREN, token_type);
		return 1;
	}
	token_type = nwslex();
	if (SEMICOLON != token_type) {
		printf ("%s: expected token type %d, got %d\n",
				test_name, SEMICOLON, token_type);
		return 1;
	}
	token_type = nwslex();
	if (O_PAREN != token_type) {
		printf ("%s: expected token type %d, got %d\n",
				test_name, O_PAREN, token_type);
		return 1;
	}
	token_type = nwslex();	
	if (COMMA != token_type) {
		printf ("%s: expected token type %d, got %d\n",
				test_name, COMMA, token_type);
		return 1;
	}
	token_type = nwslex();	/* EOF */
	if (token_type > 0) {
		printf ("%s: expected EOF, got %d\n",
				test_name, EOF, token_type);
		return 1;
	}

	printf("%s ok.\n", test_name);
	return 0;
}

int test_quoted_labels()
{
	const char *test_name = "test_quoted_labels";

	char *newick = "('abc(/)def')";
	newick_scanner_set_string_input(newick);

	int token_type = -1;

	token_type = nwslex();
	if (O_PAREN != token_type) {
		printf ("%s: expected token type %d, got %d\n",
				test_name, O_PAREN, token_type);
		return 1;
	}
	token_type = nwslex();
	if (LABEL != token_type) {
		printf ("%s: expected token type %d, got %d\n",
				test_name, LABEL, token_type);
		return 1;
	}
	if (strcmp(nwslval.sval, "'abc(/)def'") != 0) {
		printf("%s: expected label \"'abc(/)def'\", got '%s'\n",
				test_name, nwslval.sval);
		return 1;
	}
	token_type = nwslex();
	if (C_PAREN != token_type) {
		printf ("%s: expected token type %d, got %d\n",
				test_name, C_PAREN, token_type);
		return 1;
	}
	token_type = nwslex();	/* EOF */
	if (token_type > 0) {
		printf ("%s: expected EOF, got %d\n",
				test_name, EOF, token_type);
		return 1;
	}

	printf("%s ok.\n", test_name);
	return 0;
}

int test_space_in_labels()
{
	const char *test_name = "test_space_in_labels";

	char *newick = "(A space-containing label)";
	newick_scanner_set_string_input(newick);

	int token_type = -1;

	token_type = nwslex();
	if (O_PAREN != token_type) {
		printf ("%s: expected token type %d, got %d\n",
				test_name, O_PAREN, token_type);
		return 1;
	}
	token_type = nwslex();
	if (LABEL != token_type) {
		printf ("%s: expected token type %d, got %d\n",
				test_name, LABEL, token_type);
		return 1;
	}
	/* nwslex() automatically converts spaces to underscores */
	if (strcmp(nwslval.sval, "A_space-containing_label") != 0) {
		printf ("%s: expected label 'A space-containing label', got '%s'\n",
				test_name, nwslval.sval);
		return 1;
	}
	token_type = nwslex();
	if (C_PAREN != token_type) {
		printf ("%s: expected token type %d, got %d\n",
				test_name, C_PAREN, token_type);
		return 1;
	}
	token_type = nwslex();	/* EOF */
	if (token_type > 0) {
		printf ("%s: expected EOF, got %d\n",
				test_name, EOF, token_type);
		return 1;
	}

	printf("%s ok.\n", test_name);
	return 0;
}

int test_catenated_quoted_labels()
{
	const char *test_name = "test_catenated_quoted_labels";

	char *newick = "('abc''def''gh(/)ij')";
	newick_scanner_set_string_input(newick);

	int token_type = -1;

	token_type = nwslex();
	if (O_PAREN != token_type) {
		printf ("%s: expected token type %d, got %d\n",
				test_name, O_PAREN, token_type);
		return 1;
	}
	token_type = nwslex();
	if (LABEL != token_type) {
		printf ("%s: expected token type %d, got %d\n",
				test_name, LABEL, token_type);
		return 1;
	}
	if (strcmp(nwslval.sval, "'abc''def''gh(/)ij'") != 0) {
		printf ("%s: expected label \"'abc(/)def'\", got '%s'\n",
				test_name, nwslval.sval);
		return 1;
	}
	token_type = nwslex();
	if (C_PAREN != token_type) {
		printf ("%s: expected token type %d, got %d\n",
				test_name, C_PAREN, token_type);
		return 1;
	}
	token_type = nwslex();	/* EOF */
	if (token_type > 0) {
		printf ("%s: expected EOF, got %d\n",
				test_name, EOF, token_type);
		return 1;
	}

	printf("%s ok.\n", test_name);
	return 0;
}

int test_label_chars()
{
	const char *test_name = "test_label_chars";

	char *newick = "(la/bel)";
	newick_scanner_set_string_input(newick);

	int token_type = -1;

	token_type = nwslex();
	if (O_PAREN != token_type) {
		printf ("%s: expected token type %d, got %d\n",
				test_name, O_PAREN, token_type);
		return 1;
	}
	token_type = nwslex();
	if (LABEL != token_type) {
		printf ("%s: expected token type %d, got %d\n",
				test_name, LABEL, token_type);
		return 1;
	}
	if (strcmp(nwslval.sval, "la/bel") != 0) {
		printf ("%s: expected label 'la/bel', got '%s'\n",
				test_name, nwslval.sval);
		return 1;
	}
	token_type = nwslex();
	if (C_PAREN != token_type) {
		printf ("%s: expected token type %d, got %d\n",
				test_name, C_PAREN, token_type);
		return 1;
	}
	token_type = nwslex();	/* EOF */
	if (token_type > 0) {
		printf ("%s: expected EOF, got %d\n",
				test_name, EOF, token_type);
		return 1;
	}

	printf("%s ok.\n", test_name);
	return 0;
}

int test_slash_and_space()
{
	/* Part of a tree that caused the lexer to choke. Thanks to Daniel
	 * Gerlach for pointing this one out. */
	char *test_name = "test_slash_and_space";

	// This seems to work...
	/* mmap() the file wo we can pass the string to the lexer */
	int fd = open("slash_and_space.nw", O_RDONLY);
	if (-1 == fd) { perror(NULL); return 1; }
	struct stat buf;
	int status = fstat(fd, &buf);
	if (-1 == status) { perror(NULL); return 1; }
	off_t length = buf.st_size;
	char *newick = mmap(NULL, length, PROT_READ, MAP_PRIVATE,
			fd, 0);
	newick_scanner_set_string_input(newick);

	// This ought to be simpler than the mmap() way above, but does not
	// work, I don't know why.
	/*
	FILE *input = fopen("slash_and_space.nw", "r");
	if (NULL == input) { perror(NULL); return 1; }
	nwsin = input;
	*/

	int token_type = -1;
	char *exp;

	token_type = nwslex();
	if (O_PAREN != token_type) {
		printf ("%s: expected token type %d, got %d\n",
				test_name, O_PAREN, token_type);
		return 1;
	}
	token_type = nwslex();
	if (O_PAREN != token_type) {
		printf ("%s: expected token type %d, got %d\n",
				test_name, O_PAREN, token_type);
		return 1;
	}
	token_type = nwslex();
	if (LABEL != token_type) {
		printf ("%s: expected token type %d, got %d\n",
				test_name, LABEL, token_type);
		return 1;
	}
	exp = "B/Washington/05/2009_gi_255529494_gb_GQ451489";
	if (strcmp(nwslval.sval, exp) != 0) {
		printf ("%s: expected label '%s', got '%s'\n",
				test_name, exp, nwslval.sval);
		return 1;
	}
	token_type = nwslex();
	if (COLON != token_type) {
		printf ("%s: expected token type %d, got %d\n",
				test_name, COLON, token_type);
		return 1;
	}
	token_type = nwslex();
	if (LABEL != token_type) {
		printf ("%s: expected token type %d, got %d\n",
				test_name, LABEL, token_type);
		return 1;
	}
	token_type = nwslex();
	if (COMMA != token_type) {
		printf ("%s: expected token type %d, got %d\n",
				test_name, COMMA, token_type);
		return 1;
	}
	token_type = nwslex();
	if (O_PAREN != token_type) {
		printf ("%s: expected token type %d, got %d\n",
				test_name, O_PAREN, token_type);
		return 1;
	}
	token_type = nwslex();
	if (LABEL != token_type) {
		printf ("%s: expected token type %d, got %d\n",
				test_name, LABEL, token_type);
		return 1;
	}
	exp = "B/Indiana/04/2009_gi_255529556_gb_GQ451547"; 
	if (strcmp(nwslval.sval, exp) != 0) {
		printf ("%s: expected label '%s', got '%s'\n",
				test_name, exp, nwslval.sval);
		return 1;
	}
	/*
	token_type = nwslex();
	if (C_PAREN != token_type) {
		printf ("%s: expected token type %d, got %d\n",
				test_name, C_PAREN, token_type);
		return 1;
	}
	token_type = nwslex();
	if (token_type > 0) {
		printf ("%s: expected EOF, got %d\n",
				test_name, EOF, token_type);
		return 1;
	}
	*/

	printf("%s: ok.\n", test_name);
	return 0;
}

int main()
{
	int failures = 0;
	printf("Starting newick scanner test...\n");
	failures += test_simple();
	failures += test_garbled();
	failures += test_label_chars();
	failures += test_quoted_labels();
	failures += test_space_in_labels();
	failures += test_catenated_quoted_labels();
	failures += test_slash_and_space();
	if (0 == failures) {
		printf("All tests ok.\n");
	} else {
		printf("%d test(s) FAILED.\n", failures);
		return 1;
	}

	return 0;
}
