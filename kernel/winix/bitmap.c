#include "bitmap.h"
#include "slab.h"
#include <stddef.h>
#include "mem_map.h"

static unsigned long mask[BITMASK_NR];

void init_bitmap(){
	register int i;
	mask[BITMASK_NR-1] = 1;
    for(i=BITMASK_NR-2;i>=0;i--){
        mask[i] = mask[i+1] << 1;
    }
}

void bitmap_clear(unsigned long *map, int map_len){
	register int i;
	for (i=0; i < map_len; ++i)
	{
		map[i] = 0;
	}
}

void bitmap_fill(unsigned long *map, int map_len){
	register int i;
	for (i=0; i < map_len; ++i)
	{
		map[i] = 0xffffffff;
	}
}
/**
 * search from start
 * @param  map     memory map
 * @param  map_len memory map length
 * @param  start   starting bit to search from memory map, 0 <= start < 1024
 * @param  num     number of bits to search
 * @return         bit found
 */
int bitmap_search_from(unsigned long *map, int map_len, int start, int num){
	register int i,j;
	int count = 0;

	if(num >= map_len * 32 || start >= map_len * 32)	return -1;

	i = start / 32;
	j = start % 32;

	for (; i < map_len; ++i){
		for (; j< BITMASK_NR; j++) {
            if ((map[i] & mask[j]) == 0) {
                count++;
                if (count == num) {
                    return (i*32 + j - count+1);
                }
            }else{
                count = 0;
            }
        }
        j=0;
	}
	return -1;
}

int bitmap_search(unsigned long *map, int map_len, int num){
	return bitmap_search_from(map,map_len,0,num);
}

void bitmap_set_bit(unsigned long *map, int map_len,int start){
	int ibit = start/32;
	if(start >= map_len * 32)	return;
    map[ibit] = map[ibit] | mask[start%32];
}


void bitmap_set_nbits(unsigned long *map, int map_len,int start, int len){
	register int i;
	int inum;
	if(start + len >= map_len * 32)	return;
	for(i=0 ; i<len ; i++){
		inum = start + i;
		map[inum/32] = map[inum/32] | mask[inum%32];
	}
}

void bitmap_reset_nbits(unsigned long *map, int map_len,int start, int len){
	register int i;
	int inum;
	if(start + len >= map_len * 32)	return;
	for(i=0 ; i<len ; i++){
		inum = start + i;
		map[inum/32] = map[inum/32] & (~mask[inum%32]);
	}
}


void bitmap_reset_bit(unsigned long *map, int map_len,int start){
	int ibit = start/32;
	if(start >= map_len * 32)	return;
    map[ibit] = map[ibit] & (~mask[start%32]);
}

int search_backtrace(unsigned long *map, int region_len,unsigned long pattern, int pattern_len,int j){
    int i=0;
    int count = 1;
    int j_bak = j;
	unsigned long map_bit, pattern_bit;
	int result;
    unsigned long curr_pattern = pattern >> (j-1);
    for(;i<region_len;i++){
        for(;j<32;j++){
			map_bit = map[i] & mask[j];
			pattern_bit = curr_pattern & mask[j];
            result = map_bit & pattern_bit;
            // printf("%d %d %x %x %x %x %x\n",j,count,(map[i]),(map[i] & mask[j]), curr_pattern, (((curr_pattern) & mask[j])),result);
            if(result == 0){
                count++;
                if(count == pattern_len){
                    return 1;
                }
            }else{
                return 0;
            }
        }
        j=0; 
        curr_pattern = pattern << count;
    }
    return 0;
}

int bitmap_search_pattern(unsigned long *map, int map_len, unsigned long pattern, int pattern_len){
    int i=0, j = 0;
    unsigned long map_bit, pattern_bit;
	int result;
    for(i=0;i<map_len;i++){
        for(j=0;j<32;j++){
			map_bit = (map[i] & mask[j]);
			pattern_bit = (pattern >> j) & mask[j];
            result = map_bit & pattern_bit;
            // printf("%d %x %x %x %x %x\n",j,(map[i]),(map[i] & mask[j]), pattern >> j, (((pattern >> j) & mask[j])),result);
            if(result == 0){ 
                if(search_backtrace(map+i,map_len - i, pattern,pattern_len, j+1)){
                    return i*32 + j;
                }
            }
        }
    }
    return 0;
}

unsigned long createMask(unsigned long a, unsigned long b)
{
   unsigned long r = 0;
   unsigned long i;
   for (i=a; i<=b; i++)
       r |= (0x80000000 >> i);

   return r;
}

pattern_t *extract_pattern(unsigned long *map, int map_len, int heap_break){
    pattern_t *p = kmalloc(2);
    int i,j,start = 0;
    unsigned long result = 0;
    int end = (align1k(heap_break) / 1024);
    int endi = end/32;
    unsigned long tmask;
    for(i=0;i <map_len;i++){
        for(j=0; j< 32; j++){
            if((map[i] & mask[j]) == mask[j]){
                start = i*32 + j;
                goto then;
            }
        }
    }
    then:
    if(end - start > 32 || (i==31 && j==31)){
        return NULL;
    }
    result = map[i] << (j-1);
    if(i < map_len && i != endi){
        tmask = createMask(0,end%32);
        result |= (map[i+1] & tmask) >> (32 - j+1);
    }
    p->pattern = result;
    p->size = end - start;
    return p;
}