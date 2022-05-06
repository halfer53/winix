#include <stdio.h>

int putchar(int c){
    if(printf("%c",c))
        return c;
    return EOF;
}