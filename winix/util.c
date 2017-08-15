#include <kernel/kernel.h>

int align_page(int len){
    return ALIGN1K_LB(len)+PAGE_LEN;
}

/**
 * Print an error message and lock up the OS... the "Blue Screen of Death"
 *
 * Side Effects:
 *   OS locks up.
 **/
void panic(const char* str) {
	kprintf("\r\nPanic! ");
    if(str)
        kprintf(str);
	while(1) {
		RexParallel->Ctrl = 0;
		RexParallel->LeftSSD = 0x79;  //E
		RexParallel->RightSSD = 0x50; //r
	}
}

/**
 * Asserts that a condition is true.
 * If so, this function has no effect.
 * If not, panic is called with the appropriate message.
 */
void assert(int expression, int line, char* filename) {
	if(!expression) {
		kprintf("\nAssert Failed at line %d in %s",line,filename);
        panic(NULL);
	}
}



