#include <stddef.h>
char *index(const char *string, int c){
    const char *s = string;
    while(*s && *s != c)
        s++;
    return (char *)(unsigned long)(*s ? s : NULL);
}