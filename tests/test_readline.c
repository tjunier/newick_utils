#include <stdio.h>
#include <string.h>

#include "readline.h"

int test_word_tokenizer_1()
{
	const char *test_name = "test_word_tokenizer_1";

	char *line = "word1 word2 345WORD 'word 8' \"another word\" word_10";
	struct word_tokenizer *wt = create_word_tokenizer(line);

	char *word;

	word = wt_next(wt);
	if (strcmp(word, "word1") != 0) {
		printf ("%s: expected word1, got %s\n", test_name, word);
		return 1;
	}
	word = wt_next(wt);
	if (strcmp(word, "word2") != 0) {
		printf ("%s: expected word2, got %s\n", test_name, word);
		return 1;
	}
	word = wt_next(wt);
	if (strcmp(word, "345WORD") != 0) {
		printf ("%s: expected 345WORD, got %s\n", test_name, word);
		return 1;
	}
	word = wt_next(wt);
	if (strcmp(word, "'word 8'") != 0) {
		printf ("%s: expected 'word 8', got %s\n", test_name, word);
		return 1;
	}
	word = wt_next(wt);
	if (strcmp(word, "\"another word\"") != 0) {
		printf ("%s: expected \"another word\", got %s\n", test_name, word);
		return 1;
	}
	word = wt_next(wt);
	if (strcmp(word, "word_10") != 0) {
		printf ("%s: expected word_10, got %s\n", test_name, word);
		return 1;
	}
	word = wt_next(wt);
	if (NULL != word) {
		printf ("%s: expected NULL, got %s\n", test_name, word);
		return 1;
	}

	printf("%s ok.\n", test_name);
	return 0;
}

int test_word_tokenizer_2()
{
	const char *test_name = "test_word_tokenizer_2";

	char *line = "'word 1' word2\t345WORD 'word 8' \"another\tword\" word_10\n";
	struct word_tokenizer *wt = create_word_tokenizer(line);

	char *word;

	word = wt_next(wt);
	if (strcmp(word, "'word 1'") != 0) {
		printf ("%s: expected word1, got %s\n", test_name, word);
		return 1;
	}
	word = wt_next(wt);
	if (strcmp(word, "word2") != 0) {
		printf ("%s: expected word2, got %s\n", test_name, word);
		return 1;
	}
	word = wt_next(wt);
	if (strcmp(word, "345WORD") != 0) {
		printf ("%s: expected 345WORD, got %s\n", test_name, word);
		return 1;
	}
	word = wt_next(wt);
	if (strcmp(word, "'word 8'") != 0) {
		printf ("%s: expected 'word 8', got %s\n", test_name, word);
		return 1;
	}
	word = wt_next(wt);
	if (strcmp(word, "\"another\tword\"") != 0) {
		printf ("%s: expected \"another word\", got %s\n", test_name, word);
		return 1;
	}
	word = wt_next(wt);
	if (strcmp(word, "word_10") != 0) {
		printf ("%s: expected word_10, got %s\n", test_name, word);
		return 1;
	}
	word = wt_next(wt);
	if (NULL != word) {
		printf ("%s: expected NULL, got %s\n", test_name, word);
		return 1;
	}

	printf("%s ok.\n", test_name);
	return 0;
}

int test_word_tokenizer_3()
{
	const char *test_name = "test_word_tokenizer_3";

	char *line = "'word 1' word2\t345WORD 'word 8' \"another\tword\" word_10\n";
	struct word_tokenizer *wt = create_word_tokenizer(line);

	char *word;

	word = wt_next_noquote(wt);
	if (strcmp(word, "word 1") != 0) {
		printf ("%s: expected word1, got %s\n", test_name, word);
		return 1;
	}
	word = wt_next_noquote(wt);
	if (strcmp(word, "word2") != 0) {
		printf ("%s: expected word2, got %s\n", test_name, word);
		return 1;
	}
	word = wt_next_noquote(wt);
	if (strcmp(word, "345WORD") != 0) {
		printf ("%s: expected 345WORD, got %s\n", test_name, word);
		return 1;
	}
	word = wt_next_noquote(wt);
	if (strcmp(word, "word 8") != 0) {
		printf ("%s: expected 'word 8', got %s\n", test_name, word);
		return 1;
	}
	word = wt_next_noquote(wt);
	if (strcmp(word, "another\tword") != 0) {
		printf ("%s: expected \"another word\", got %s\n", test_name, word);
		return 1;
	}
	word = wt_next_noquote(wt);
	if (strcmp(word, "word_10") != 0) {
		printf ("%s: expected word_10, got %s\n", test_name, word);
		return 1;
	}
	word = wt_next_noquote(wt);
	if (NULL != word) {
		printf ("%s: expected NULL, got %s\n", test_name, word);
		return 1;
	}

	printf("%s ok.\n", test_name);
	return 0;
}

int test_readline_1()
{
	const char *test_name = "test_readline_1";

	FILE *input = fopen("readline_test.txt", "r");

	char *line, *exp;

	exp = "A simple line.";
	line = read_line(input);
	if (strcmp(exp, line) != 0) {
		printf ("%s: expected '%s', got '%s'\n", test_name, exp, line);
		return 1;
	}

	exp = "Another line.";
	line = read_line(input);
	if (strcmp(exp, line) != 0) {
		printf ("%s: expected '%s', got '%s'\n", test_name, exp, line);
		return 1;
	}


	printf("%s ok.\n", test_name);
	return 0;
}

int test_readline_2()
{
	const char *test_name = "test_readline_2";

	FILE *input = fopen("color.map", "r");

	char *line, *exp;

	exp = "# Comments start with a '#' and are ignored.";
	line = read_line(input);
	if (strcmp(exp, line) != 0) {
		printf ("%s: expected '%s', got '%s'\n", test_name, exp, line);
		return 1;
	}

	exp = "# Empty lines are ignored too:";
	line = read_line(input);
	if (strcmp(exp, line) != 0) {
		printf ("%s: expected '%s', got '%s'\n", test_name, exp, line);
		return 1;
	}

	exp = "";
	line = read_line(input);
	if (strcmp(exp, line) != 0) {
		printf ("%s: expected '%s', got '%s'\n", test_name, exp, line);
		return 1;
	}

	exp = "# ...as are lines consisting only of whitespace:";
	line = read_line(input);
	if (strcmp(exp, line) != 0) {
		printf ("%s: expected '%s', got '%s'\n", test_name, exp, line);
		return 1;
	}

	exp = "   \t\t\t ";
	line = read_line(input);
	if (strcmp(exp, line) != 0) {
		printf ("%s: expected '%s', got '%s'\n", test_name, exp, line);
		return 1;
	}

	exp = "stroke:blue CLADE Homo Pan";
	line = read_line(input);
	if (strcmp(exp, line) != 0) {
		printf ("%s: expected '%s', got '%s'\n", test_name, exp, line);
		return 1;
	}

	exp = "stroke:red CLADE Cercopithecus Macaca Papio";
	line = read_line(input);
	if (strcmp(exp, line) != 0) {
		printf ("%s: expected '%s', got '%s'\n", test_name, exp, line);
		return 1;
	}

	exp = "stroke:green CLADE Macaca Simias";
	line = read_line(input);
	if (strcmp(exp, line) != 0) {
		printf ("%s: expected '%s', got '%s'\n", test_name, exp, line);
		return 1;
	}

	exp = "";
	line = read_line(input);
	if (strcmp(exp, line) != 0) {
		printf ("%s: expected '%s', got '%s'\n", test_name, exp, line);
		return 1;
	}

	exp = "# an empty line at the end no longer causes Segv:";
	line = read_line(input);
	if (strcmp(exp, line) != 0) {
		printf ("%s: expected '%s', got '%s'\n", test_name, exp, line);
		return 1;
	}


	exp = "";
	line = read_line(input);
	if (strcmp(exp, line) != 0) {
		printf ("%s: expected '%s', got '%s'\n", test_name, exp, line);
		return 1;
	}

	exp = "";
	line = read_line(input);
	if (strcmp(exp, line) != 0) {
		printf ("%s: expected '%s', got '%s'\n", test_name, exp, line);
		return 1;
	}

	printf("%s ok.\n", test_name);
	return 0;
}

int main()
{
	int failures = 0;
	printf("Starting readline test...\n");
	failures += test_word_tokenizer_1();
	failures += test_word_tokenizer_2();
	failures += test_word_tokenizer_3();
	failures += test_readline_1();
	failures += test_readline_2();
	if (0 == failures) {
		printf("All tests ok.\n");
	} else {
		printf("%d test(s) FAILED.\n", failures);
		return 1;
	}

	return 0;
}
