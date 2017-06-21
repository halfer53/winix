/**
 * WINIX Idle Task.
 *
 * Revision History:
 *  2016-09-19		Paul Monigatti			Original
 *  2016-11-20		Bruce Tan			Modified
 **/
 
#include <winix/rex.h>

#define DELAY 20000

// Simple animation sequence for the SSDs.
int pattern[] = { 0x1000, 0x2000, 0x0100, 0x0001, 0x0002, 0x0004, 0x0008, 0x0800 };

/**
 * IDLE entry point
 **/
void idle_main() {
	//TODO: this task should just loop forever, with no side effects.
	//Currently, a test pattern is shown on the SSDs.
	
	int i = 0;
	unsigned int delay = 0;
	kprintf("IDLE");
	
	while(1) {
		RexParallel->Ctrl = 0;
		RexParallel->LeftSSD = pattern[i] >> 8;
		RexParallel->RightSSD = pattern[i];
		
		if(++i == sizeof(pattern))
			i = 0;
		
		delay = DELAY;
		while(--delay);
	}
}
