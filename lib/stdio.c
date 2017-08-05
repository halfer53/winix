#include <lib.h>

void perror(const char *s){
    if(s)
        printf("%s",s);
    printf(" errno val: %d\n",errno);
}

int putc(const char c){
    return printf("%c",c);
}
