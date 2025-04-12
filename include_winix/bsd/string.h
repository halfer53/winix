#ifndef _BSD_STRING_H_
#define _BSD_STRING_H_

#include <stddef.h>

size_t strlcpy(char *dest, const char *src, size_t n);
size_t strlcat(char *dest, const char *src, size_t n);

#endif

