#include <sys/types.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "rnode.h"
#include "to_newick.h"
#include "link.h"

int test_trivial()
{
	const char *test_name = "test_trivial";
	char *result;
	struct rnode *node_a;
	char * exp = "a;";

	node_a = create_rnode("a", "");
	is_leaf(node_a);
	result = to_newick(node_a);
	if (strcmp(exp, result) != 0) {
		printf("%s: expected '%s', got '%s'.\n",
				test_name, exp, result);
		return 1;
	}
	printf("%s: ok.\n", test_name);
	return 0;
	
}

int test_simple_1()
{
	const char *test_name = "test_simple_1";
	char *result;
	struct rnode *node_a;
	struct rnode *node_b;
	char *exp = "(a:12);";

	node_a = create_rnode("a", "12");
	node_b = create_rnode("", "");
	add_child(node_b, node_a);

	result = to_newick(node_b);
	if (strcmp(exp, result) != 0) {
		printf("%s: expected '%s', got '%s'.\n",
				test_name, exp, result);
		return 1;
	}
	printf("%s: ok.\n", test_name);
	return 0;
}

int test_simple_2()
{
	const char *test_name = "test_simple_2";
	char *result;
	struct rnode *node_a;
	struct rnode *node_b;
	struct rnode *node_c;
	char *exp = "(a:12,b:5);";

	node_a = create_rnode("a", "12");
	node_b = create_rnode("b", "5");
	node_c = create_rnode("", "");
	add_child(node_c, node_a);
	add_child(node_c, node_b);

	result = to_newick(node_c);
	if (strcmp(exp, result) != 0) {
		printf("%s: expected '%s', got '%s'.\n",
				test_name, exp, result);
		return 1;
	}
	printf("%s: ok.\n", test_name);
	return 0;
}

/* (a,(b,c)d)e; */
int test_nested_1()
{
	const char *test_name = "test_nested_1";
	char *result;
	struct rnode *node_a;
	struct rnode *node_b;
	struct rnode *node_c;
	struct rnode *node_d;
	struct rnode *node_e;
	char *exp = "(a,(b,c));";

	node_a = create_rnode("a", NULL);
	node_b = create_rnode("b", NULL);
	node_c = create_rnode("c", NULL);
	node_d = create_rnode("", NULL);
	node_e = create_rnode("", NULL);

	add_child(node_d, node_b);
	add_child(node_d, node_c);
	add_child(node_e, node_a);
	add_child(node_e, node_d);

	result = to_newick(node_e);
	if (strcmp(exp, result) != 0) {
		printf("%s: expected '%s', got '%s'.\n",
				test_name, exp, result);
		return 1;
	}
	printf("%s: ok.\n", test_name);
	return 0;
}

/* (a:12,(b:5,c:7)d:3)e; */
int test_nested_2()
{
	const char *test_name = "test_nested_2";
	char *result;
	struct rnode *node_a;
	struct rnode *node_b;
	struct rnode *node_c;
	struct rnode *node_d;
	struct rnode *node_e;
	char *exp = "(a:12,(b:5,c:7)d:3)e;";

	node_a = create_rnode("a", "12");
	node_b = create_rnode("b", "5");
	node_c = create_rnode("c", "7");
	node_d = create_rnode("d", "3");
	node_e = create_rnode("e", "");

	add_child(node_d, node_b);
	add_child(node_d, node_c);
	add_child(node_e, node_a);
	add_child(node_e, node_d);

	result = to_newick(node_e);
	if (strcmp(exp, result) != 0) {
		printf("%s: expected '%s', got '%s'.\n",
				test_name, exp, result);
		return 1;
	}
	printf("%s: ok.\n", test_name);
	return 0;
}

/* bug 1 - this bug appeared when testing other functions */

int test_bug1()
{
	const char *test_name = "test_bug1";

	struct rnode *parent;
	struct rnode *child_1;
	struct rnode *child_2;
	struct rnode *child_3;
	struct rnode *child_4;

	parent = create_rnode("parent", "");
	child_1 = create_rnode("child_1", "");
	child_2 = create_rnode("child_2", "");
	child_3 = create_rnode("child_3", "");
	child_4 = create_rnode("child_4", "");

	add_child(parent, child_1);
	add_child(parent, child_2);
	add_child(parent, child_3);
	add_child(parent, child_4);
	
	struct rnode *new_child = create_rnode("new", "");

	replace_child(parent, child_3, new_child);

	char *exp = "(child_1,child_2,new,child_4)parent;";
	if (0 != strcmp(exp, to_newick(parent))) {
		printf ("%s: expected '%s', got '%s'\n",
			test_name, exp, to_newick(parent));
		return 1;
	}

	printf("%s ok.\n", test_name);
	return 0;
}

int test_dump_simple()
{
	const char *test_name = "test_dump_simple";
	char *result;
	struct rnode *node_a;
	struct rnode *node_b;
	char *exp = "(a:12);";

	node_a = create_rnode("a", "12");
	node_b = create_rnode("", "");
	add_child(node_b, node_a);

	/* dump_newick() writes to standard output. If we want to check what it
	 * wrote, we have to redirect its output to a file. This can be done
	 * easily with dup() or dup2(). But this is not the end of the story,
	 * because we want to resume writing to the old stdout after
	 * dump_newick() has done its job -- we have to print a summary of how
	 * the test went, and of course there are other tests down te line. I
	 * found two ways of doing this: */

	/* This works, and conforms to C99, but what if the terminal isn't
	 * /dev/tty? */
	/*
	FILE *out = freopen("test.out", "w", stdout);
	dump_newick(node_b);
	freopen("/dev/tty", "w", out);
	*/

	/* This also works, and may be more portable, but involves fork()ing a
	 * child process: overkill? */
	int fd = creat("test.out", S_IWUSR | S_IRUSR);
	if (-1 == fd) { perror(NULL); return 1; }
	pid_t pid = fork();
	if (-1 == pid) { perror(NULL); return 1; }
	if (0 == pid) {
		/* child */
		close(STDOUT_FILENO);
		dup(fd);
		dump_newick(node_b);
		exit(EXIT_SUCCESS);
	} else {
		/* parent */
		wait(NULL);
	}

	// TODO here: read (mmap?) the output file, and compare it with the
	// expected Newick string. This should replace the following block.
	result = to_newick(node_b);
	if (strcmp(exp, result) != 0) {
		printf("%s: expected '%s', got '%s'.\n",
				test_name, exp, result);
		return 1;
	}

	printf("%s: ok.\n", test_name);
	return 0;
}

int main()
{
	int failures = 0;
	printf("Starting to_newick test...\n");
	failures += test_trivial();
	failures += test_simple_1();
	failures += test_simple_2();
	failures += test_nested_1();
	failures += test_nested_2();
	failures += test_bug1();
	failures += test_dump_simple();
	if (0 == failures) {
		printf("All tests ok.\n");
	} else {
		printf("%d test(s) FAILED.\n", failures);
		return 1;
	}

	return 0;
}
