/*
 * regerror
 */
#include <stdio.h>
#include <stdlib.h>

void
regerror(s)
char *s;
{
#ifdef ERRAVAIL
	error("regexp: %s", s);
#else
	fprintf(stderr, "regexp(3): %s\n", s);
	exit(EXIT_FAILURE);
#endif
	/* NOTREACHED */
}
