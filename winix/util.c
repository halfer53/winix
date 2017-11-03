/**
 * 
 * Micellaneous utilities
 *
 * @author Bruce Tan
 * @email brucetansh@gmail.com
 * 
 * @author Paul Monigatti
 * @email paulmoni@waikato.ac.nz
 * 
 * @create date 2017-08-23 06:13:08
 * 
*/
#include <kernel/kernel.h>
#include <winix/rex.h>

int align_page(int len){
    return ALIGN1K_LB(len)+PAGE_LEN;
}

/**
 * Print an error message and lock up the OS... the "Blue Screen of Death"
 *
 * Side Effects:
 *   OS locks up.
 **/
void _panic(const char* str, const char* file) {
    kprintf("\r\nPanic! ");

    if(str)
        kprintf(str);

    if(file)
        kprintf(" in %s\n", file);
        
    while(1) {
        RexParallel->Ctrl = 0;
        RexParallel->LeftSSD = 0x79;  // E
        RexParallel->RightSSD = 0x50; // r
    }
}

/**
 * Asserts that a condition is true.
 * If so, this function has no effect.
 * If not, panic is called with the appropriate message.
 */
void _assert(int expression, int line, char* filename) {
    if(!expression) {
        kprintf("\nAssert Failed at line %d in %s",line,filename);
        _panic(NULL, NULL);
    }
}



