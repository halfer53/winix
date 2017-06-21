#include "string.h"
/*

 */
void *memcpy(void *s1, const void *s2, register int n)
{
    register char *p1 = s1;
    register const char *p2 = s2;

    if (n) {
        n++;
        while (--n > 0) {
            *p1++ = *p2++;
        }
    }
    return s1;
}

/**
 * Compares two strings.
 **/
int strcmp(const char *s1, const char *s2) {
    while (*s1 && *s2) {
        if (*s1 != *s2) {
            break;
        }
        s1++;
        s2++;
    }

    return *s1 - *s2;
}

/**
 * Calculates the length of a string.
 **/
//int strlen(const char *s) {
//    int len = 0;
//
//    while (*s++) {
//        len++;
//    }
//
//    return len;
//}

/**
 * copy the src into the destination string, the destination should be large enough to hold the src
 **/
char *strcpy(char *dest, const char *src) {
    char *saved = dest;
    while (*src) {
        *dest++ = *src++;
    }
    *dest = 0;
    return saved;
}

/**
 * append the src to the end of the dest, the dest should be large enough to hold extra src
 **/
char *strcat(char *dest, const char *src) {
    int i = 0, src_index = 0;
    while (dest[i] != '\0') i++;

    for (; src[src_index] != '\0'; src_index++) {
        dest[i + src_index] = src[src_index];
    }
    dest[i + src_index] = '\0';
    return dest;
}

/**
 * return the length of substring of the original string, starting at start_index, and has specified length
 **/
int Substring(char* buffer, char* original, int start_index, int length) {
    int i = 0;
    int count = 0;
    for (i = start_index; i < length + start_index; i++)
    {
        buffer[count] = original[i];
        count++;
    }
    buffer[count] = '\0';
    return count;
}
