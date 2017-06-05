/**
 * REX Hardware mappings.
 *
 * Revision History:
 *  2016-09-19		Paul Monigatti			Original
 *  2016-11-20		Bruce Tan			Modified
 **/
 
#ifndef _REX_H_
#define _REX_H_

/**
 * REX Serial Ports
 **/
typedef volatile struct
{
	int Tx;
	int Rx;
	int Ctrl;
	int Stat;
	int Iack;
} RexSp_t;

/**
 * REX Timer
 **/
typedef volatile struct
{
	int Ctrl;
	int Load;
	int Count;
	int Iack;
} RexTimer_t;
 
/**
 * REX Parallel Port
 **/
typedef volatile struct
{
	int Switches;
	int Buttons;
	int LeftSSD;
	int RightSSD;
	int Ctrl;
	int Iack;
} RexParallel_t;

/**
 * REX User Interrupt Button
 **/
typedef volatile struct
{
	int Iack;
} RexUserInt_t;

/**
 * Declarations
 **/
#define RexSp1 		((RexSp_t*)0x70000)
#define RexSp2 		((RexSp_t*)0x71000)
#define RexTimer  	((RexTimer_t*)0x72000)
#define RexParallel	((RexParallel_t*)0x73000)
#define RexUserInt	((RexUserInt_t*)0x7f000)

#endif
