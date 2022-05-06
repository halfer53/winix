#include <stddef.h>
/**
 * Calculates the length of a string.
 **/
size_t strlen(const char *s) {
    const char* bak = s;
    for (; *s; ++s)
                ;
    return s - bak;
}