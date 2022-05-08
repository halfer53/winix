#ifndef _BSD_STRING_H_
#define _BSD_STRING_H_

#include <stddef.h>

char *strlcpy(char *dest, const char *src, size_t n);
char *strlcat(char *dest, const char *src, size_t n);

#endif

