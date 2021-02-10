/**
 * 
 * Simple parallel test program for WINIX.
 *
 * @author Bruce Tan
 * @email brucetansh@gmail.com
 * 
 * @author Paul Monigatti
 * @email paulmoni@waikato.ac.nz
 * 
 * @create date 2016-09-19
 * 
*/
#include <kernel/kernel.h>
#include <winix/rex.h>

// TODO: remove rex.h include

/**
 * What base should we display the number as?
 **/
typedef enum { HEX, DEC } displayBase;
static displayBase _mode = HEX;

/**
 * Prints a number to the SSDs in base 16
 *
 * Parameters:
 *   n        The number to print.
 **/
void printHex(int n) {
    RexParallel->LeftSSD = (n >> 4) & 0xf;
    RexParallel->RightSSD = (n & 0xf);
}

/**
 * Prints a number to the SSDs in base 10
 *
 * Parameters:
 *   n        The number to print.
 **/
void printDec(int n) {
    RexParallel->LeftSSD = (n / 10) % 10;
    RexParallel->RightSSD = n % 10;
}

/**
 * Main entry point.
 **/
void parallel_main() {
    int switch_val, n;
    RexParallel->Ctrl = 1; // HEX-SSD decoding on
 
    while(1) {
        // Load switch value
        switch_val = RexParallel->Switches;
        if(!switch_val)
            n = 0;
        else
            n = 100 / switch_val;
        
        // Change display mode?
        switch(RexParallel->Buttons) {
            case 1:
                _mode = HEX;
                break;
                
            case 2:
                _mode = DEC;
                break;
            
            case 3:
                task_exit();
                return; // exit
        }
        
        // Display the number
        switch(_mode) {
            case HEX:
                printHex(n);
                break;
                
            case DEC:
                printDec(n);
                break;
        }
    }
}
