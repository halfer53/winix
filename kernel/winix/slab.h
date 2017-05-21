#ifndef _K_SLAB_H_
#define _K_SLAB_H_


void* kmalloc (size_t size);
void kfree(void *ptr);
void* krealloc(void *p,size_t size);
void *kcalloc(size_t number , size_t size);


#endif
