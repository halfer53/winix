#include <stdlib.h>

void *calloc(size_t number , size_t size) {
	size_t *new;
	size_t s4, i;
	new = malloc(number * size);
	if (new) {
		s4 = number * size;
		for (i = 0; i < s4 ; i++)
			new[i] = 0;
	}
	return (new);
}