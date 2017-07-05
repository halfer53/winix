#ifndef _W_SLAB_H_
#define _W_SLAB_H_ 1

#include <winix/gfp.h>

void* kmalloc (unsigned long size);
void kfree(void *ptr);
void* krealloc(void *p,unsigned long size);
void *kcalloc(unsigned long number , unsigned long size);
void kblock_overview();
void init_slab(void *addr, int size);


#endif
