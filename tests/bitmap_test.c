#include <assert.h>
#include <winix/bitmap.h>

extern unsigned int mask[BITMASK_NR];

void test_given_bitmask_should_return_aligned(){
    int i;
    unsigned int curr = 0x80000000;
    for(i=1; i <= BITMASK_NR; i++){
        assert(mask[i-1] == curr);
        curr = curr >> 1;
    }
}