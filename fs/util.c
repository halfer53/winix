//
// Created by bruce on 27/04/20.
//
#include "fs.h"

char32_t *char32_strlcpy(char32_t *dest, const char *src, size_t n){
    char32_t *saved = dest;
    while (*src && n--) {
        *dest++ = *src++;
    }
    *dest = 0;
    return saved;
}

int char32_strlen(const char32_t *s) {
    const char32_t* bak = s;
    for (; *s; ++s)
            ;
    return (int)(s - bak);
}

int char32_strcmp(const char32_t *s1, const char *s2) {
    while (*s1 && *s2) {
        if (*s1 != *s2) {
            break;
        }
        s1++;
        s2++;
    }

    return (int)(*s1 - *s2);
}


