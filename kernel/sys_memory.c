#include "winix.h"


//scan the FREE_MEM_BEGIN
void Scan_FREE_MEM_BEGIN() {
	//TODO start kmalloc here
	FREE_MEM_BEGIN = (size_t)&BSS_END;

	//Round up to the next 1k boundary
	FREE_MEM_BEGIN |= 0x03ff;
	FREE_MEM_BEGIN++;

	// kprintf("\r\nfree memory begin %x\r\n", FREE_MEM_BEGIN );
}


void *expand_mem(size_t size) {
	size_t temp = FREE_MEM_BEGIN;
	if (FREE_MEM_END != 0) {
		//if FREE_MEM_END is not null, then that means the OS is running
		//otherwise it's initialising, thus FREE_MEM_END is not set yet
		//we just assume there is enough memory during the start up
		//since calculating FREE_MEM_END during the start up is gonna crash the system for some unknown reason
		if (size + FREE_MEM_BEGIN > FREE_MEM_END) {
			return NULL;
		}
	}

	FREE_MEM_BEGIN += size;
	//kprintf("free mem %x\n",FREE_MEM_BEGIN );
	return (void *)temp;
}







