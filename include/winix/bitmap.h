/**
 * 
 * bitmap utilities
 *
 * @author Bruce Tan
 * @email brucetansh@gmail.com
 * 
 * @author Paul Monigatti
 * @email paulmoni@waikato.ac.nz
 * 
 * @create date 2017-08-23 06:21:46
 * 
*/
#ifndef _BITMAP_H_
#define _BITMAP_H_ 1

#include <stdbool.h>

#define BITMASK_NR    32

struct bit_pattern{
    unsigned int pattern;
    int size;
};

#define set_bit(map,i)      ((map) |= (1 << (i)))
#define unset_bit(map,i)    ((map) &= ~(1 << (i)))

void init_bitmap();
int get_mask(int i);
bool is_bit_on(unsigned int *map, int map_len, int bit);
int bitmap_clear(unsigned int *map, int map_len);
int bitmap_fill(unsigned int *map, int map_len);

int bitmap_search_from(unsigned int *map, int map_len, int start, int num);
int bitmap_search_reverse(unsigned int *map, int map_len, int num);
int bitmap_set_bit(unsigned int *map, int map_len,int start);
int bitmap_set_nbits(unsigned int *map, int map_len,int start, int len);
int bitmap_clear_bit(unsigned int *map, int map_len,int start);
int bitmap_clear_nbits(unsigned int *map, int map_len,int start, int len);
int bitmap_xor(unsigned int *map1, unsigned int *map2, int size_len);


int bitmap_search_pattern(unsigned int *map, int map_len, unsigned int pattern, int pattern_len);
int bitmap_set_pattern(unsigned int *map, int map_len, int index, unsigned int pattern, int pattern_len);
int bitmap_extract_pattern(unsigned int *map, int map_len, int heap_break, struct bit_pattern* ptn);

int count_bits(unsigned int *map, int map_len, int flags);
void kreport_bitmap(unsigned int *p, int len);


#define bitmap_search(map,map_len,num)  bitmap_search_from((map),(map_len), 0 , (num))

#endif

