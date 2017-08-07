#include <kernel/kernel.h>

unsigned int mask[BITMASK_NR];

void init_bitmap(){
	register int i;
	mask[BITMASK_NR-1] = 1;
    for(i=BITMASK_NR-2;i>=0;i--){
        mask[i] = mask[i+1] << 1;
    }
}

int bitmap_clear(unsigned int *map, int map_len){
	register int i;
	for (i=0; i < map_len; ++i)
	{
		map[i] = 0;
	}
    return OK;
}

int bitmap_fill(unsigned int *map, int map_len){
	register int i;
	for (i=0; i < map_len; ++i)
	{
		map[i] = 0xffffffff;
	}
    return OK;
}
/**
 * search from start
 * @param  map     memory map
 * @param  map_len memory map length
 * @param  start   starting bit to search from memory map, 0 <= start < 1024
 * @param  num     number of bits to search
 * @return         bit found
 */
int bitmap_search_from(unsigned int *map, int map_len, int start, int num){
	register int i,j;
	int count = 0;

	if(num >= map_len * 32 || start >= map_len * 32)
        return ERR;

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
	return ERR;
}

int bitmap_search(unsigned int *map, int map_len, int num){
	return bitmap_search_from(map,map_len,0,num);
}

int bitmap_search_reverse(unsigned int *map, int map_len, int num){
	register int i,j;
	int count = 0;

	if(num >= map_len * 32 )
        return ERR;

	for (i = map_len -1; i >= 0; i--){
		for (j = BITMASK_NR -1; j >= 0; j--) {
            if ((map[i] & mask[j]) == 0) {
                count++;
                if (count == num) {
                    return (i*32 + j);
                }
            }else{
                count = 0;
            }
        }
	}
	return ERR;
}

int bitmap_set_bit(unsigned int *map, int map_len,int start){
	int ibit = start/32;
	if(start >= map_len * 32)	
        return ERR;
    map[ibit] = map[ibit] | mask[start%32];
    return OK;
}


int bitmap_set_nbits(unsigned int *map, int map_len,int start, int len){
	register int i;
	int inum;
	if(start + len >= map_len * 32)	
        return ERR;
	for(i=0 ; i<len ; i++){
		inum = start + i;
		map[inum/32] = map[inum/32] | mask[inum%32];
	}
    return OK;
}

int bitmap_clear_nbits(unsigned int *map, int map_len,int start, int len){
	register int i;
	int inum;
	if(start + len >= map_len * 32)	
        return ERR;
	for(i=0 ; i<len ; i++){
		inum = start + i;
		map[inum/32] = map[inum/32] & (~mask[inum%32]);
	}
    return OK;
}


int bitmap_clear_bit(unsigned int *map, int map_len,int start){
	int ibit = start/32;
	if(start >= map_len * 32)	
        return ERR;
    map[ibit] = map[ibit] & (~mask[start%32]);
    return OK;
}

int search_backtrace(unsigned int *map, int region_len,unsigned int pattern, int pattern_len,int j){
    int i=0;
    int count = 1;
    int j_bak = j;
	unsigned int map_bit, pattern_bit;
	int result;
    unsigned int curr_pattern = pattern >> (j-1);
    for(;i<region_len;i++){
        for(;j<32;j++){
			map_bit = map[i] & mask[j];
			pattern_bit = curr_pattern & mask[j];
            result = map_bit & pattern_bit;
            // kprintf(" %d |",j);
            if(result == 0){
                count++;
                if(count == pattern_len){
                    return ERR;
                }
            }else{
                return OK;
            }
        }
        j=0; 
        curr_pattern = pattern << count;
    }
    return OK;
}

int bitmap_search_pattern(unsigned int *map, int map_len,int start, unsigned int pattern, int pattern_len){
    int i=0, j = 0;
    unsigned int map_bit, pattern_bit;
	int result;
    for(i=0;i<map_len;i++){
        for(j=0;j<32;j++){
			map_bit = (map[i] & mask[j]);
			pattern_bit = (pattern >> j) & mask[j];
            result = map_bit & pattern_bit;
            // kprintf("%d 0x%08x 0x%08x 0x%08x 0x%08x\n",j,(map[i]),map_bit, pattern_bit ,result);
            if(result == 0){ 
                if(pattern_len == 1)
                    return i*32+j;
                if(search_backtrace(map+i,map_len - i, pattern,pattern_len, j+1)){
                    return i*32 + j;
                }
            }
        }
    }
    return OK;
}

unsigned int createMask(unsigned int a, unsigned int b)
{
   unsigned int r = 0;
   unsigned int i;
   for (i=a; i<=b; i++)
       r |= (0x80000000 >> i);

   return r;
}

int bitmap_extract_pattern(unsigned int *map, int map_len, int heap_break, pattern_t *p){
    int i,j,start = 0;
    unsigned int result = 0;
    int end = (align1k(heap_break) / 1024);
    int endi = end/32;
    unsigned int tmask;
    for(i=0;i < map_len;i++){
        for(j=0; j< 32; j++){
            if((map[i] & mask[j]) == mask[j]){
                start = i*32 + j;
                // kprintf(" i %d j %d\n",i,j);
                goto then;
            }
        }
    }
    
    then:
    if(end - start > 32 || (i==31 && j==31)){
        return ERR;
    }
    result = map[i] << (j);
    if(i < map_len && i != endi){
        tmask = createMask(0,end%32);
        result |= (map[i+1] & tmask) >> (32 - j);
    }
    // kprintf(" start %d end %d\n",start, end);
    p->pattern = result;
    p->size = end - start +1;
    return OK;
}

int bitmap_set_pattern(unsigned int *map, int map_len, int index, unsigned int pattern, int pattern_len){
    int i= index/32, j=index%32;
    map[i] |= (pattern >> j);
    if(i < map_len -1 && 32 - j < pattern_len){
        map[i+1] |= (pattern << (32 - j));
    }
    return OK;
}


int bitmap_xor(unsigned int *map1, unsigned int *map2, int map_len){
    int i=0;
    for(i = 0; i< map_len; i++){
        map1[i] ^= map2[i];
    }
    return OK;
}
