#include <stdio.h>
#include <stdlib.h>

void __assert(int expression, int line, char* filename){
    if (!expression){
        fprintf(stderr, "assertion failed at %s:%d\n", filename, line);
        abort();
    }
}
