/* A program for exploring ideas */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>

#include "tree.h"
#include "parser.h"
#include "to_newick.h"
#include "rnode.h"
#include "rnode_iterator.h"

void get_params(int argc, char *argv[])
{
	if (2 == argc) {
		if (0 != strcmp("-", argv[1])) {
			FILE *fin = fopen(argv[1], "r");
			extern FILE *nwsin;
			if (NULL == fin) {
				perror(NULL);
				exit(EXIT_FAILURE);
			}
			nwsin = fin;
		}
	} else {
		fprintf(stderr, "Usage: %s <filename|->\n",
				argv[0]);
		exit(EXIT_FAILURE);
	}
}

int main(int argc, char *argv[])
{
	struct rooted_tree *tree;

	char *name = "tests/color.map";
	FILE *file = fopen(name, "r");
	if (NULL == file) {perror(NULL); exit(EXIT_FAILURE);}
	char *line = NULL;
	int line_buf_size = 0;
	
	int bytes_read;
	while ((bytes_read = getline(&line, &line_buf_size, file)) != -1) {
		printf("bytes read: %d\n", bytes_read);
		printf("contents: %s\n", line);
	}
	free(line);
	return 0;
}
