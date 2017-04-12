/**
 * Simple parallel test program for WINIX.
 *
 * Revision History:
 *  2016-09-19		Paul Monigatti			Original
 *  2016-11-20		Bruce Tan			Modified
 **/

#include <sys/rex.h>
//TODO: remove rex.h include

/**
 * What base should we display the number as?
 **/
typedef enum { HEX, DEC } displayBase;
static displayBase _mode = HEX;

/**
 * Prints a number to the SSDs in base 16
 *
 * Parameters:
 *   n		The number to print.
 **/
void printHex(int n) {
	RexParallel->LeftSSD = (n >> 4) & 0xf;
	RexParallel->RightSSD = (n & 0xf);
}

/**
 * Prints a number to the SSDs in base 10
 *
 * Parameters:
 *   n		The number to print.
 **/
void printDec(int n) {
	RexParallel->LeftSSD = (n / 10) % 10;
	RexParallel->RightSSD = n % 10;
}

/**
 * Main entry point.
 **/
void parallel_main() {
	RexParallel->Ctrl = 1; //HEX-SSD decoding on
	
	while(1) {
		
		//Load switch value
		int n = 100 / RexParallel->Switches;
		
		//Change display mode?
		switch(RexParallel->Buttons) {
			case 1:
				_mode = HEX;
				break;
				
			case 2:
				_mode = DEC;
				break;
			
			case 3:
				sys_exit(0);
				return; //exit
		}
		
		//Display the number
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
