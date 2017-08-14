#ifndef _W_SLAB_H_
#define _W_SLAB_H_ 1

#include <winix/gfp.h>

void* kmalloc (unsigned int size);
void kfree(void *ptr);
void* krealloc(void *p,unsigned int size);
void *kcalloc(unsigned int number , unsigned int size);
void kblock_overview();
void init_slab(void *addr, int size);


#endif
