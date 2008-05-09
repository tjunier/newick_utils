%{
#include <string.h>

int indent_level = 0;
char *indent_string;
void indent(const int, const char*);
char *eol = "\n";

/* we may peform additional operations just after printing a tree */
void cap_tree(char *eol)
{
	if (0 == strcmp(eol, "")) 	/* compact tree */
		printf("\n");	
}

%}

%%

('[^']*'|[a-zA-Z0-9_.+-]+)?(:[0-9.]+)?,?	{
	indent(indent_level, indent_string);
	printf("%s%s", yytext, eol);
}
"("	{
	indent(indent_level, indent_string);
	printf("(%s", eol); indent_level++;
}
\)('[^']*'|[a-zA-Z0-9_.+-]+)?(:[0-9.]+)?[,;]?	{
	indent_level--;
	indent(indent_level, indent_string);
	printf("%s%s", yytext, eol);
	if (0 == indent_level) cap_tree(eol);
}
\[[^]]*]	/* ignore comments */ ;
[\t\n ]+ ;

%%

void indent(const int level, const char *indent_string)
{
	int i;
	for (i = 0; i < level; i++)
		printf("%s", indent_string);
}
