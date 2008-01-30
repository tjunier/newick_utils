#include <stdio.h>
#include <string.h>
#include <stdlib.h>

char *concat(char *s1, char *s2)
{
	size_t l1, l2;
	char *result;

	l1 = strlen(s1);
	l2 = strlen(s2);
	
	if (NULL == (result = malloc((l1+l2+1) * sizeof(char)))) {
		return NULL;
	}
	strcpy(result, s1);
	strcat(result, s2);

	return result;
}

char *append_to(char *s1, char *s2)
{
	size_t l1 = strlen(s1);
	size_t l2 = strlen(s2);

	char * result = realloc(s1, (l1+l2+1) * sizeof(char));
	if (NULL == result) {
		return NULL;
	}
	strcpy(result+l1, s2);

	return result;
}
