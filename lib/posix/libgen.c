#include <stddef.h>

char *basename(char *str){
    char *last_slash = NULL, *filename = NULL;
    while (*str){
        if (*str == '/'){
            last_slash = str;
        }
        str++;
    }
    if (last_slash){
        filename = last_slash + 1;
    }
    return filename;
}
