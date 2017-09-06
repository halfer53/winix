/**
 * 
 * Mem alloc
 *
 * @author Bruce Tan
 * @email brucetansh@gmail.com
 * 
 * @author Paul Monigatti
 * @email paulmoni@waikato.ac.nz
 * 
 * @create date 2017-08-23 06:22:10
 * 
*/
#ifndef _MEM_ALLOC_H_
#define _MEM_ALLOC_H_ 1

/**

IMPORTANT:
depreciated, use slab.c instead

**/

#ifndef _TYPE_H_
#include <sys/types.h>
#endif

struct hole{
  size_t *start;
  unsigned int length;
  struct hole *next;
};

#define NUM_HOLES 100

void hole_list_overview();

void *mem_alloc(size_t size);
void mem_free(void *ptr_parameter);
void init_holes();

#endif
