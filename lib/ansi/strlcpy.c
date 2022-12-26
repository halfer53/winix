#include <stddef.h>

char *strlcpy(char *dest, const char *src, size_t n){
    char *saved = dest;
    n++;
    while (*src && n--) {
        *dest++ = *src++;
    }
    *dest = 0;
    return saved;
}
