/* reroot: rename tree nodes according to a map */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "tree.h"
#include "parser.h"
#include "to_newick.h"
#include "hash.h"
#include "list.h"
#include "rnode.h"



struct parameters {
	char  *map_filename;
};

/* Returns a line from a file, as a pointer to an allocated buffer */

char * readline(FILE *file)
{
	char *line;
	long fpos;
	long len = 0L;
	int c;
	
	/* return NULL if EOF */
	if (feof(file)) return NULL;

	fpos = ftell(file);	/* remember where we start */
	if (-1 == fpos) {
		perror(NULL);
		exit(EXIT_FAILURE);
	}

	/* find next newline */
	while ((c = getc(file)) != EOF) {
		if ('\n' == c)
			break;
		len++;
	}

	/* return NULL if EOF and line length is 0 */
	if (feof(file) && 0 == len) return NULL;

	/* allocate memory for line */
	line = malloc((1 + len) * sizeof(char));
	if (NULL == line) {
		perror(NULL);
		exit(EXIT_FAILURE);
	}

	/* return to where we started */
	if (-1 == fseek(file, fpos, SEEK_SET)) {
		perror(NULL);
		exit(EXIT_FAILURE);
	}

	fgets(line, len+1, file);

	/* consumes newline (otherwise gets stuck here...) */
	fgetc(file);

	return line;
}

struct hash *read_map(const char *filename)
{
	const int HASH_SIZE = 1000;	/* most trees will have fewer nodes */

	FILE *map_file = fopen(filename, "r");
	if (NULL == map_file) {
		perror(NULL);
		exit(EXIT_FAILURE);
	}

	struct hash *map = create_hash(HASH_SIZE);
	char *line;
	while (NULL != (line = readline(map_file))) {
		/* Start of line is start of key, we just need to find key'end
		 * and start of value. */
		char *p, *value;
		p = strpbrk(line, " \t");	/* find first whitespace */
		*p = '\0';			/* terminate key */
		p++;				
		int skip = (int) strspn(p, " \t"); /* next non-whitespace */
		value = p + skip;		/* no need for \0 (fgets()) */
		hash_set(map, line, (void *) value);
	}

	return map;
}

struct parameters get_params(int argc, char *argv[])
{

	struct parameters params;

	/* TODO: uncomment this when we add switches and options */
	/* parse options and switches */
	/*
	int opt_char;
	while ((opt_char = getopt(argc, argv, "dw:")) != -1) {
		switch (opt_char) {
		case 'w':
			params.width = strtod(optarg, NULL);
			if (0 == params.width) {
				fprintf(stderr,
			"Argument to -w must be a positive integer.\n");
				exit(EXIT_FAILURE);
			}
			break;
		case 'd':
			params.debug = 1;
		}
	}
	*/

	/* check arguments */
	if ((argc - optind) == 2)	{
		if (0 != strcmp("-", argv[optind])) {
			FILE *fin = fopen(argv[optind], "r");
			extern FILE *yyin;
			if (NULL == fin) {
				perror(NULL);
				exit(EXIT_FAILURE);
			}
			yyin = fin;
		}
		params.map_filename = argv[2];
	} else {
		fprintf(stderr, "Usage: %s <filename|-> <map_filename>\n",
				argv[0]);
		exit(EXIT_FAILURE);
	}

	return params;
}

int main(int argc, char *argv[])
{
	struct rooted_tree *tree;	
	struct hash *rename_map;
	struct parameters params;
	
	params = get_params(argc, argv);

	rename_map = read_map(params.map_filename);
	tree = parse_tree();

	/* visit each node, and change name if needed */
	struct list_elem *elem;
	for (elem = tree->nodes_in_order->head; NULL != elem; elem = elem->next) {
		struct rnode *current = (struct rnode *) elem->data;
		char *label = current->label;
		char *new_label = hash_get(rename_map, label);
		if (NULL != new_label) {
			current->label = new_label;
		}
	}

	printf ("%s\n", to_newick(tree->root));

	return 0;
}
