/**
 * 
 * kernel slab
 *
 * @author Bruce Tan
 * @email brucetansh@gmail.com
 * 
 * @author Paul Monigatti
 * @email paulmoni@waikato.ac.nz
 * 
 * @create date 2017-08-23 06:22:32
 * 
*/
#ifndef _W_SLAB_H_
#define _W_SLAB_H_ 1

#include <winix/gfp.h>

void* kmalloc(unsigned int size);
void kfree(void *ptr);

void* _kmalloc(unsigned int size, void *ra);
void _kfree(void *ptr, void *ra);
void* krealloc(void *p,unsigned int size);
void *kcalloc(unsigned int number , unsigned int size);
void kkreport_mallinfo();
void init_slab(void *addr, int size);

#endif
