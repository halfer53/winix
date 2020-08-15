#include <lib.h>

FILE _stdin = { 0, 0, 0, 0 , 0, 0};
FILE _stdout = { 0, 1, 0, 0 , 0, 0};
FILE _stderr = { 0, 2, 0, 0 , 0, 0};
FILE *stdin = &_stdin, *stdout = &_stdout, *stderr = &_stderr;

int putchar(const char c){
    if(printf("%c",c))
        return c;
    return EOF;
}


