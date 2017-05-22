#ifndef _K_SLAB_H_
#define _K_SLAB_H_


void* kmalloc (unsigned long size);
void kfree(void *ptr);
void* krealloc(void *p,unsigned long size);
void *kcalloc(unsigned long number , unsigned long size);
void block_overview();


#endif
