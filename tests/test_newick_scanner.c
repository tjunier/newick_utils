#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h> 
#include <string.h>

#include "rnode.h"
#include "list.h"
#include "parser.h"
#include "newick_parser.h"

extern FILE *nwsin;

/* These could be put in a header file (e.g. parser.h), but they are not really
 * meant to be public (they are used only in the parser), so I just declare
 * them here instead. TODO: well, maybe parser.h is a good idea... some
 * functions /are/ used elsewhere, e.g. newick_scanner_set_string_input().  */

int nwslex (void);
struct rnode *root;
struct llist *nodes_in_order;
enum parser_status_type newick_parser_status;
void newick_scanner_set_string_input(char *);
void newick_scanner_set_file_input(FILE *);

int test_simple()
{
	const char *test_name = "test_simple";

	/* Note: this is a valid Newick string, but since we're checking the
	 * scanner, it doesn't have to be - see test_garbled()  */
	char *input = "((A,B),C);";
	newick_scanner_set_string_input(input);

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
				test_name, token_type);
		return 1;
	}

	printf("%s ok.\n", test_name);
	return 0;
}

int test_garbled()
{
	const char *test_name = "test_garbled";

	char *input = ")ABC(;(,";
	newick_scanner_set_string_input(input);

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
				test_name, token_type);
		return 1;
	}

	printf("%s ok.\n", test_name);
	return 0;
}

int test_quoted_labels()
{
	const char *test_name = "test_quoted_labels";

	char *input = "('abc(/)def')";
	newick_scanner_set_string_input(input);

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
				test_name, token_type);
		return 1;
	}

	printf("%s ok.\n", test_name);
	return 0;
}

int test_space_in_labels()
{
	const char *test_name = "test_space_in_labels";

	char *input = "(A space-containing label)";
	newick_scanner_set_string_input(input);

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
				test_name, token_type);
		return 1;
	}

	printf("%s ok.\n", test_name);
	return 0;
}

int test_catenated_quoted_labels()
{
	const char *test_name = "test_catenated_quoted_labels";

	char *input = "('abc''def''gh(/)ij')";
	newick_scanner_set_string_input(input);

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
				test_name, token_type);
		return 1;
	}

	printf("%s ok.\n", test_name);
	return 0;
}

int test_label_chars()
{
	const char *test_name = "test_label_chars";

	char *input = "(la/bel)";
	newick_scanner_set_string_input(input);

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
				test_name, token_type);
		return 1;
	}

	printf("%s ok.\n", test_name);
	return 0;
}

int test_slash_and_space()
{
	/* Part of a tree that caused the lexer to choke. Thanks to Daniel
	 * Gerlach for pointing this one out. Also spaces to the labels, to
	 * make test case stricter.*/
	char *test_name = "test_slash_and_space";
	
	FILE *input = fopen("slash_and_space.nw", "r");
	if (NULL == input) { perror(NULL); return 1; }
	newick_scanner_set_file_input(input);

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

int test_comments()
{
	const char *test_name = "test_comments";

	char *input = "[comment](a,(b,c));[another comment]";
	newick_scanner_set_string_input(input);

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
	if (strcmp(nwslval.sval, "a") != 0) {
		printf ("%s: expected label 'a', got '%s'\n",
				test_name, nwslval.sval);
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
	if (strcmp(nwslval.sval, "b") != 0) {
		printf ("%s: expected label 'b', got '%s'\n",
				test_name, nwslval.sval);
		return 1;
	}
	token_type = nwslex();
	if (COMMA != token_type) {
		printf ("%s: expected token type %d, got %d\n",
				test_name, COMMA, token_type);
		return 1;
	}
	token_type = nwslex();
	if (LABEL != token_type) {
		printf ("%s: expected token type %d, got %d\n",
				test_name, LABEL, token_type);
		return 1;
	}
	if (strcmp(nwslval.sval, "c") != 0) {
		printf ("%s: expected label 'c', got '%s'\n",
				test_name, nwslval.sval);
		return 1;
	}
	token_type = nwslex();
	if (C_PAREN != token_type) {
		printf ("%s: expected token type %d, got %d\n",
				test_name, C_PAREN, token_type);
		return 1;
	}
	token_type = nwslex();
	if (C_PAREN != token_type) {
		printf ("%s: expected token type %d, got %d\n",
				test_name, C_PAREN, token_type);
		return 1;
	}
	token_type = nwslex();
	if (SEMICOLON != token_type) {
		printf ("%s: expected token type %d, got %d\n",
				test_name, SEMICOLON, token_type);
		return 1;
	}
	token_type = nwslex();	/* EOF */
	if (token_type > 0) {
		printf ("%s: expected EOF, got %d\n",
				test_name, EOF);
		return 1;
	}

	printf("%s ok.\n", test_name);
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
	failures += test_comments();
	if (0 == failures) {
		printf("All tests ok.\n");
	} else {
		printf("%d test(s) FAILED.\n", failures);
		return 1;
	}

	return 0;
}
