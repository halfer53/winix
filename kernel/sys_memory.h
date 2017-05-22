#ifndef _SYS_MEMORY_H_
#define _SYS_MEMORY_H_

typedef struct _hole{
  size_t *start;
  unsigned long length;
  struct _hole *next;
}hole_t;

#define NUM_HOLES 100
extern size_t SYS_BSS_START;

static void hole_enqueue_tail(hole_t **q, hole_t *hole);
static void hole_enqueue_head(hole_t **q, hole_t *hole);
static hole_t *hole_dequeue(hole_t **q);
void hole_list_overview();
void Scan_FREE_MEM_BEGIN();
void *_sbrk(proc_t *caller,size_t size);
void *proc_malloc(size_t size);
void init_memory();
void *expand_mem(size_t size);


#endif
