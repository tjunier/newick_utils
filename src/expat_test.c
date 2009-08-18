#include <stdio.h>
#include <string.h>
#include <expat.h>

#include "list.h"
#include "link.h"
#include "rnode.h"
#include "masprintf.h"
#include "to_newick.h"

enum elem {CLADE, NAME};

typedef struct {
	enum elem type;
	void *data;
	char *text;
	unsigned int text_length;
} StackElem;

static struct llist *stack;
static size_t XML_Char_size = sizeof(XML_Char);

static void XMLCALL startElement(void *userData, const char *name,
		const char **atts)
{
	StackElem *st_elem = malloc(sizeof(StackElem));
	if (NULL == st_elem) {perror(NULL), exit(EXIT_FAILURE);}
	st_elem->text= NULL;
	st_elem->text_length = 0;
	int i;
	for (i = 0; i < stack->count; i++) putchar(' ');
	prepend_element(stack, st_elem);
	printf("unshifted <%s>, count is %d\n", name, stack->count);
}

static void XMLCALL textData (void *userData, const XML_Char *s,
		int len)
{
	StackElem *st_elem = stack->head->data;
	int new_text_length = st_elem->text_length + len;
	st_elem->text = realloc(st_elem->text, new_text_length);
	if (NULL == st_elem->text) { perror(NULL); exit(EXIT_FAILURE); }
	memcpy((st_elem->text+st_elem->text_length), s, len);
}

static void XMLCALL endElement(void *userData, const char *name)
{
		
	StackElem *st_elem = shift(stack);
	int i;
	st_elem->text = realloc(st_elem->text, st_elem->text_length+1);
	if (NULL == st_elem->text) { perror(NULL); exit(EXIT_FAILURE); }
	st_elem->text[st_elem->text_length] = '\0';
	for (i = 0; i < stack->count; i++) putchar(' ');
	printf("shifted </%s>, count is %d, text is '%s'\n",
			name, stack->count, st_elem->text);
	free(st_elem->text);
	free(st_elem);
}

int main(int argc, char* argv[])
{
	const int BUF_SIZE = 1024;

	stack = create_llist();

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

	destroy_llist(stack);

	return 0;
}

