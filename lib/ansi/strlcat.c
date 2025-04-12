#include <stddef.h>

size_t strlcat(char *dest, const char *src, size_t n) {
    char* saved = dest;
    size_t dst_len;
    while(*dest++);
    dest--;
    dst_len = (size_t)(dest - saved);
    n -= dst_len;
    if(n <= 0){
        return 0;
    }
    while (*src && n--) {
        *dest++ = *src++;
    }
    *dest = '\0';
    return dest - saved;
}