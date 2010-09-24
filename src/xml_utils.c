/* 

Copyright (c) 2009 Thomas Junier and Evgeny Zdobnov, University of Geneva
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice,
    this list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright notice,
    this list of conditions and the following disclaimer in the documentation
    and/or other materials provided with the distribution.
* Neither the name of the University of Geneva nor the names of its
    contributors may be used to endorse or promote products derived from this
    software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <libxml/tree.h>
#include <libxml/parser.h>
#include <libxml/xpath.h>

#include "masprintf.h"

/* Wraps an SVG snipped in a dummy document (have to do this for the snippet to
 * parse as an XML doc, unless there happens to be just one element). */

static char * wrap_in_dummy_doc(const char *svg_snippet)
{
	const char * dummy_doc_start = "<dummy xmlns:xlink=\"http://www.w3.org/1999/xlink\">";
	const char * dummy_doc_stop = "</dummy>";
	char *doc_str;


	/* wrap snippet into dummy document */
	size_t start_length = strlen(dummy_doc_start);
	size_t snippet_length = strlen(svg_snippet);
	size_t stop_length = strlen(dummy_doc_stop);
	size_t doc_length = start_length + snippet_length + stop_length;
	/* + 1 for trailing '\0' */
	doc_str = malloc((doc_length + 1) * sizeof(char));
	if (NULL == doc_str) return NULL;

	strcpy(doc_str, dummy_doc_start);
	strcpy(doc_str + start_length, svg_snippet);
	strcpy(doc_str + start_length + snippet_length, dummy_doc_stop);

	return doc_str;
}

/* changes the x-attribute's sign */
// TODO: these will be superseded by f()s in svg_graph_radial.c
static void chs_x_attr(xmlDocPtr doc, char *attr)
{
	/* look for xpath attributes in elements */
	xmlChar *xpath = (xmlChar *) masprintf("//*[@%s]", attr);
	xmlXPathContextPtr context = xmlXPathNewContext(doc);
	xmlXPathObjectPtr result = xmlXPathEvalExpression(xpath, context);
	if (xmlXPathNodeSetIsEmpty(result->nodesetval)) {
		xmlXPathFreeObject(result);
		xmlXPathFreeContext(context);
		free(xpath);
		return;
	} else {
		xmlNodeSetPtr nodeset = result->nodesetval;
		int i;
		for (i=0; i < nodeset->nodeNr; i++) {
			xmlNodePtr node = nodeset->nodeTab[i];
			xmlChar *value = xmlGetProp(node, (xmlChar *) attr);
			if (NULL != value) {
				double x_val = atof((char *) value);
				x_val *= -1.0;
				char *new_value = masprintf("%g", x_val);
				xmlSetProp(node, (xmlChar *) attr,
						(xmlChar *) new_value);
				free(new_value);
			}	
			xmlFree(value);
		}
		xmlXPathFreeObject (result);
	}
	free(xpath);
	xmlXPathFreeContext(context);
}	

static char *unwrap_snippet(xmlDocPtr doc)
{
	/* this will give a size that is certain to be enough */
	xmlChar *xml_buf;
	int buf_length;
	xmlDocDumpFormatMemory(doc, &xml_buf, &buf_length, 0); 
	xmlFree(xml_buf);	/* only need buf_length */
	/* so, allocate that much (cleared) */
	char *tweaked_svg = calloc(buf_length, sizeof(char));
	if (NULL == tweaked_svg) return NULL;
	// TODO: the following 2 lines can be merged
	xmlNodePtr cur = xmlDocGetRootElement(doc);
	cur = cur->xmlChildrenNode;
	while (NULL != cur) {
		xmlBufferPtr buf = xmlBufferCreate ();
		xmlNodeDump (buf, doc, cur, 0, 0);
		const xmlChar * contents = xmlBufferContent(buf);
		int cur_len = strlen(tweaked_svg);
		/* appends to tweaked_svg - cur_len must initially be 0, which
		 * is why we use calloc() */
		strcpy(tweaked_svg + cur_len, (char *) contents);
		xmlBufferFree(buf);
		cur = cur->next;	/* sibling */
	}
	return tweaked_svg;
}

char *change_svg_x_attr_sign(const char *svg_snippet)
{
	xmlDocPtr doc;

	char *wrapped_snippet = wrap_in_dummy_doc(svg_snippet);
	if (NULL == wrapped_snippet) { perror(NULL); exit(EXIT_FAILURE); }

	/* parse SVG from string */
	doc = xmlParseMemory(wrapped_snippet, strlen(wrapped_snippet));
	if (NULL == doc) {
		fprintf(stderr, "Failed to parse document\n");
		return NULL;
	}
	free(wrapped_snippet);

	/* See http://www.w3.org/TR/SVG/attindex.html for a list of
	 * x-attributes. */
	chs_x_attr(doc, "cx"); /* circle, ellipse, radialGradient */
	chs_x_attr(doc, "fx"); /* radialGradient */
	// TODO: couldn't we get the same effect by using 'dx' on <text>, etc?
	chs_x_attr(doc, "dx"); /* text, tref, tspan, ... */
	chs_x_attr(doc, "x"); /* text, image ... */
	chs_x_attr(doc, "x1"); /* line, ... */
	chs_x_attr(doc, "x2"); /* line, ... */

	/* now print out the altered snipped, unwrapped.  */
	char *tweaked_svg = unwrap_snippet(doc);
	xmlFreeDoc(doc);

	return tweaked_svg;
}

char *escape_predefined_character_entities(const char *string)
{
	const int MAX_ESCAPE_LEN = 6;	/* e.g. "&quot;" is 6 chars long */

	char *s = strdup(string); /* local copy: don't touch the const
				     argument */

	/* This allocates for enough characters even if all chars in the
	 * argument have to be escaped, and all of them have the longest
	 * possible escape  - like this we're absolutely sure our escaped
	 * string will fit. And don't worry, we're going to free the unused
	 * memory. */
	char *result = malloc(MAX_ESCAPE_LEN * strlen(string) * sizeof(char));
	if (NULL == result) 
		return NULL;

	char *c;			/* scans s */
	char *result_end = result;	/* points to last char copied */
	for (c = s; *c != '\0'; c++) {
		switch (*c) {
		case '&':
			strncpy(result_end, "&amp;", 5);
			result_end += 5;
			break;
		case '<':
			strncpy(result_end, "&lt;", 4);
			result_end += 4;
			break;
		case '>':
			strncpy(result_end, "&gt;", 4);
			result_end += 4;
			break;
		case '\'':
			strncpy(result_end, "&apos;", 6);
			result_end += 6;
			break;
		case '"':
			strncpy(result_end, "&quot;", 6);
			result_end += 6;
			break;
		default:
			*result_end = *c;
			result_end++;
			break;
		}
	}
	*result_end = '\0';
	free(s);

/* Cygwin's realloc() chokes on this, I'm not sure why. I understand that it is
 * dangerous to ignore the return value (as the data may have been moved),
 * but I do update the 'return' pointer to the new value. Oh well, we'll have
 * go with a little excess memory under Cygwin, unless I find out what's going
 * on here. */

#ifndef __CYGWIN__
	size_t new_length = strlen(result) + 1;
	result = realloc(result, new_length); /* NULL handled by caller */
#endif
	return result;
}
