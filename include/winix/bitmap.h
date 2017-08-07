#ifndef _BITMAP_H_
#define _BITMAP_H_ 1

#define BITMASK_NR	32

typedef struct pattern_b{
    unsigned int pattern;
    int size;
}pattern_t;

extern unsigned int mask[BITMASK_NR];

void init_bitmap();
int bitmap_clear(unsigned int *map, int map_len);
int bitmap_fill(unsigned int *map, int map_len);

int bitmap_search(unsigned int *map, int map_len, int num);
int bitmap_search_reverse(unsigned int *map, int map_len, int num);
int bitmap_set_bit(unsigned int *map, int map_len,int start);
int bitmap_set_nbits(unsigned int *map, int map_len,int start, int len);
int bitmap_clear_bit(unsigned int *map, int map_len,int start);
int bitmap_clear_nbits(unsigned int *map, int map_len,int start, int len);
int bitmap_xor(unsigned int *map1, unsigned int *map2, int size_len);


int bitmap_search_pattern(unsigned int *map, int map_len, int start,unsigned int pattern, int pattern_len);
int bitmap_set_pattern(unsigned int *map, int map_len, int index, unsigned int pattern, int pattern_len);
int bitmap_extract_pattern(unsigned int *map, int map_len, int heap_break, pattern_t* ptn);


#endif

