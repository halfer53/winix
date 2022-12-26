#include <stdlib.h>
#include <bsd/string.h>
#include <string.h>

char *strdup(const char *s) {
    size_t len = strlen(s) + 1;
    char *d = malloc(len);
    if (d == NULL) {
        return NULL;
    }
    strlcpy(d, s, len);
    return d;
}