#include <stddef.h>

void *memcpy(void *s1, const void *s2, size_t n)
{
    char *p1 = s1;
    const char *p2 = s2;

    if (n) {
        n++;
        while (--n > 0) {
            *p1++ = *p2++;
        }
    }
    return s1;
}
