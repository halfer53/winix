#ifndef _BITMAP_H_
#define _BITMAP_H_

#define BITMASK_NR	32

typedef struct pattern_b{
    unsigned int pattern;
    int size;
}pattern_t;

extern unsigned long mask[BITMASK_NR];

void init_bitmap();
void bitmap_clear(unsigned long *map, int map_len);
void bitmap_fill(unsigned long *map, int map_len);

int bitmap_search(unsigned long *map, int map_len, int num);
int bitmap_search_reverse(unsigned long *map, int map_len, int num);
void bitmap_set_bit(unsigned long *map, int map_len,int start);
void bitmap_set_nbits(unsigned long *map, int map_len,int start, int len);
void bitmap_reset_bit(unsigned long *map, int map_len,int start);
void bitmap_reset_nbits(unsigned long *map, int map_len,int start, int len);
void bitmap_xor(unsigned long *map1, unsigned long *map2, int size_len);


int bitmap_search_pattern(unsigned long *map, int map_len, int start,unsigned long pattern, int pattern_len);
void bitmap_set_pattern(unsigned long *map, int map_len, int index, unsigned long pattern, int pattern_len);
int bitmap_extract_pattern(unsigned long *map, int map_len, int heap_break, pattern_t* ptn);

#endif

