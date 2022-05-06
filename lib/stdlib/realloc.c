#include "memblock.h"
#include <stdlib.h>

void* realloc(void *p, long size){
    char* ret, *dst;
    char *ptr = p;
    int len;
    struct mem_control_block *mcb;
    mcb = (struct mem_control_block*)(ptr - sizeof(struct mem_control_block));
    ret = malloc(size);
    len = mcb->size;
    dst = ret;
    while(len--){
        *dst++ = *ptr++;
    }
    free(p);
    return ret;
}