#ifndef _BITMAP_H_
#define _BITMAP_H_

#define BITMASK_NR	32

void init_bitmap();
void bitmap_reset_all(unsigned long *map, int map_len);
void bitmap_set_all(unsigned long *map, int map_len);

int bitmap_search(unsigned long *map, int map_len, int num);
void bitmap_set_bit(unsigned long *map, int map_len,int start);
void bitmap_set_nbits(unsigned long *map, int map_len,int start, int len);
void bitmap_reset_bit(unsigned long *map, int map_len,int start);
void bitmap_reset_nbits(unsigned long *map, int map_len,int start, int len);

#endif

