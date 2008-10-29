/* Common definitions, etc. */

// TODO: use C99's booleans
#define FALSE 0
#define TRUE 1
#define HERE {printf ("**** up to %s, %d ****\n", __FILE__, __LINE__); fflush(stdin);}
