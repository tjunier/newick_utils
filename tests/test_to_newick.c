#include <sys/types.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "rnode.h"
#include "to_newick.h"
#include "link.h"
#include "concat.h"
#include "parser.h"
#include "tree.h"

/* dump_newick() writes to standard output. If we want to check what it wrote,
 * we have to redirect its output to a file. This can be done easily with dup()
 * or dup2(). But this is not the end of the story, because we want to resume
 * writing to the old stdout after dump_newick() has done its job -- we have to
 * print a summary of how the test went, and of course there are other tests
 * down the line. I found two ways of doing this: */

int dump_newick_to_file(struct rnode *node, char *outname)
{

	/* This works, and conforms to C99, but what if the terminal isn't
	 * /dev/tty? */
	/*
	FILE *out = freopen("test.out", "w", stdout);
	dump_newick(node_b);
	freopen("/dev/tty", "w", out);
	*/

	/* This also works, and may be more portable, but involves fork()ing a
	 * child process: overkill? Moreover, DON'T redirect output (as in make
	 * check), as it will interfere with the test. TODO: try to fix this
	 * problem. Reproduce by doing i) ./test_to_newick, and ii)
	 * ./test_to_newick | grep -i fail -- in the second case there will be
	 * (false-positive) failures due to redirection */
	int fd = open(outname, O_CREAT | O_TRUNC | O_RDWR, S_IWUSR | S_IRUSR);
	if (-1 == fd) return -1;
	pid_t pid = fork();
	if (-1 == pid) return -1; 
	if (0 == pid) {
		/* child */
		close(STDOUT_FILENO);
		if (-1 == dup(fd)) {
			perror("dup");
			close(fd);
			exit(EXIT_FAILURE);
		}
		dump_newick(node);
		close(fd);
		exit(EXIT_SUCCESS);
	} else {
		/* parent */
		wait(NULL);
		return fd;
	}
}

char *mmap_output(int fd)
{
	struct stat buf;
	if (-1 == fstat(fd, &buf)) return NULL;
	char *output = mmap(NULL, buf.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
	if (NULL == output) return NULL;

	return output;
}

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
	char *test_name = "test_dump_simple";
	char *result;
	struct rnode *node_a;
	struct rnode *node_b;
	char *exp = "(a:12);\n";

	node_a = create_rnode("a", "12");
	node_b = create_rnode("", "");
	add_child(node_b, node_a);

	char *outname = concat(test_name, ".out");
	if (NULL == outname) { perror(NULL); return 1; }

	/* dump the newick into a file (by redirecting stdout) */
	int fd = dump_newick_to_file(node_b, outname);
	if (-1 == fd) { perror(NULL); return 1; }

	/* read the output into a string */
	char *obt = mmap_output(fd);
	if (NULL == obt) { perror(NULL); return 1; }

	if (strcmp(exp, obt) != 0) {
		printf("%s: expected '%s', got '%s'.\n", test_name, exp, obt);
		return 1;
	}

	printf("%s: ok.\n", test_name);
	unlink(outname);
	return 0;
}

/* (a:12,(b:5,c:7)d:3)e; */
int test_dump_nested_2()
{
	char *test_name = "test_dump_nested_2";
	char *result;
	struct rnode *node_a;
	struct rnode *node_b;
	struct rnode *node_c;
	struct rnode *node_d;
	struct rnode *node_e;
	char *exp = "(a:12,(b:5,c:7)d:3)e:4;\n";

	node_a = create_rnode("a", "12");
	node_b = create_rnode("b", "5");
	node_c = create_rnode("c", "7");
	node_d = create_rnode("d", "3");
	node_e = create_rnode("e", "4");
	add_child(node_d, node_b);
	add_child(node_d, node_c);
	add_child(node_e, node_a);
	add_child(node_e, node_d);

	char *outname = concat(test_name, ".out");
	if (NULL == outname) { perror(NULL); return 1; }

	int fd = dump_newick_to_file(node_e, outname);
	if (-1 == fd) { perror(NULL); return 1; }

	char *obt = mmap_output(fd);
	if (NULL == obt) { perror(NULL); return 1; }

	if (strcmp(exp, obt) != 0) {
		printf("%s: expected '%s', got '%s'.\n",
				test_name, exp, obt);
		return 1;
	}

	printf("%s: ok.\n", test_name);
	unlink(outname);
	return 0;
}

int test_bug2()
{
	char *test_name = "test_bug2";
	char *newick = "(Bovine:0.69395,(Gibbon:0.36079,(Orang:0.33636,(Gorilla:0.17147,(Chimp:0.19268,Human:0.11927):0.08386):0.06124):0.15057):0.54939,Mouse:1.21460):0.10;";

	newick_scanner_set_string_input(newick);

	struct rooted_tree *tree = parse_tree();
	struct rnode *root = tree->root;
	if (strcmp("", root->label) != 0) {
		printf("%s: expected empty root label, got '%s'\n",
				test_name, root->label);
		return 1;
	}
	if (strcmp("0.10", root->edge_length_as_string) != 0) {
		printf("%s: expected root edge length 0.10 , got '%s'\n",
				test_name, root->edge_length_as_string);
		return 1;
	}
	char *obt = to_newick(tree->root);
	if (strcmp(obt, newick) != 0) {
		printf ("%s: expected '%s', got '%s'\n", test_name,
				newick, obt);
		return 1;
	}

	printf("%s ok.\n", test_name);
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
	failures += test_bug2();
	failures += test_dump_simple();
	failures += test_dump_nested_2();
	if (0 == failures) {
		printf("All tests ok.\n");
	} else {
		printf("%d test(s) FAILED.\n", failures);
		return 1;
	}

	return 0;
}
