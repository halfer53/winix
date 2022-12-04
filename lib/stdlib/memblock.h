#ifndef _MEMBLOCK
#define _MEMBLOCK

struct mem_control_block {
  int is_available;
  int size;
  struct mem_control_block *prev;
};

#endif
