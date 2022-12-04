#include <stdio.h>
#include <stdlib.h>

ssize_t getdelim(char **lineptr, size_t *n, int delimiter, FILE *stream)
{
    int c;
    size_t i;

    if (lineptr == NULL || n == NULL || stream == NULL) {
        return -1;
    }

    if (*lineptr == NULL) {
        *n = 120;
        *lineptr = (char *)malloc(*n);
        if (*lineptr == NULL) {
            return -1;
        }
    }

    i = 0;
    while ((c = fgetc(stream)) != EOF) {
        if (i + 1 >= *n) {
            *n *= 2;
            *lineptr = (char *)realloc(*lineptr, *n);
            if (*lineptr == NULL) {
                return -1;
            }
        }
        (*lineptr)[i++] = c;
        if (c == delimiter) {
            break;
        }
    }

    if (i == 0 && c == EOF) {
        return -1;
    }

    (*lineptr)[i] = '\0';
    return i;
}