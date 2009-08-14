#include <stdio.h>
#include <string.h>
#include <expat.h>

#include "list.h"
#include "link.h"
#include "rnode.h"
#include "masprintf.h"
#include "to_newick.h"

static struct llist *clade_stack;
static void *text_buf;
static size_t text_buf_size;
static size_t XML_Char_size = sizeof(XML_Char);

static void XMLCALL startElement(void *userData, const char *name,
		const char **atts)
{
	static int num = 0;
	if (strcmp("clade", name) == 0) {
		char *num_s = masprintf("node_%d", num);
		struct rnode *clade = create_rnode(num_s, "");
		prepend_element(clade_stack, clade);
		printf ("pushed %s\n", num_s);
		free(num_s);
		num++;
	}
	if (strcmp("name", name) == 0) {
		text_buf = calloc(0, XML_Char_size);
		text_buf_size = 0;
		if (NULL == text_buf) {perror(NULL), exit(EXIT_FAILURE);}
	}
}

static void XMLCALL textData (void *userData, const XML_Char *s,
		int len)
{
	memcpy((text_buf + text_buf_size), s, len * XML_Char_size);
	text_buf_size += len;
}

static void XMLCALL endElement(void *userData, const char *name)
{
	if (strcmp("clade", name) == 0) {
		struct rnode *child = shift(clade_stack);
		if (clade_stack->count > 0) {
			struct rnode *parent = clade_stack->head->data;
			printf ("popped %s\n", child->label);
			add_child(parent, child);
			printf ("added to %s\n", parent->label);
		} else {
			printf ("%s\n", to_newick(child));
		}
	} else if (strcmp("name", name) == 0) {
		free(text_buf);
	}

}

int main(int argc, char* argv[])
{
	const int BUF_SIZE = 1024;

	clade_stack = create_llist();

	char buf[BUF_SIZE];
	XML_Parser parser = XML_ParserCreate(NULL);
	XML_SetElementHandler(parser, startElement, endElement);
	XML_SetCharacterDataHandler(parser, textData);
	int end;
	do {
		size_t bytes_read = fread(buf, 1, sizeof(buf), stdin);
		end = ((int) bytes_read) < sizeof(buf);
		if (ferror(stdin)) {
			fprintf(stderr, "Problem reading input\n");
			exit(EXIT_FAILURE);
		}
		if (XML_Parse(parser, buf, bytes_read, end) ==
				XML_STATUS_ERROR) {
			fprintf(stderr, "%s at line %u\n",
		      		XML_ErrorString(XML_GetErrorCode(parser)),
			    	(unsigned int) XML_GetCurrentLineNumber(parser));
			exit(EXIT_FAILURE);
		}
	} while (!end);
	XML_ParserFree(parser);

	destroy_llist(clade_stack);

	return 0;
}

