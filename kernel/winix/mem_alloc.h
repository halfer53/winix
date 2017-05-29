#ifndef _MEM_ALLOC_H_
#define _MEM_ALLOC_H_

#include <type.h>

typedef struct _hole{
  size_t *start;
  unsigned long length;
  struct _hole *next;
}hole_t;

#define NUM_HOLES 100
extern size_t SYS_BSS_START;

void hole_list_overview();

void *mem_alloc(size_t size);
void mem_free(void *ptr_parameter);
void init_holes();

#endif
