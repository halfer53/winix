#include <kernel/kernel.h>

int align_page(int len){
    return ALIGN1K_LB(len)+PAGE_LEN;
}



