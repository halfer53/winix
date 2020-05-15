#include <lib.h>

int putchar(const char c){
    if(printf("%c",c))
        return c;
    return EOF;
}
