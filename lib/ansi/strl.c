#include <stddef.h>

char *strlcat(char *dest, const char *src, size_t n) {
    char* saved = dest;
    size_t dst_len;
    while(*dest++);
    dest--;
    dst_len = (size_t)(dest - saved);
    n -= dst_len;
    if(n <= 0){
        return saved;
    }
    while (*src && n--) {
        *dest++ = *src++;
    }
    *dest = '\0';
    return saved;
}


char *strlcpy(char *dest, const char *src, size_t n){
    char *saved = dest;
    n++;
    while (*src && n--) {
        *dest++ = *src++;
    }
    *dest = 0;
    return saved;
}
