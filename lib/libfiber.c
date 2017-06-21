/* On Mac OS X, _XOPEN_SOURCE must be defined before including ucontext.h.
Otherwise, getcontext/swapcontext causes memory corruption. See:

http://lists.apple.com/archives/darwin-dev/2008/Jan/msg00229.html */
#ifdef __APPLE__
#define _XOPEN_SOURCE
#endif

#include "libfiber.h"

#include <stdlib.h>
#include <ucontext.h>

/* The Fiber Structure
*  Contains the information about individual fibers.
*/
typedef struct
{
	ucontext_t context; /* Stores the current context */
	int active; /* A boolean flag, 0 if it is not active, 1 if it is */
	/* Original stack pointer. On Mac OS X, stack_t.ss_sp is changed. */
	void* stack; 
} fiber;

/* The fiber "queue" */
static fiber fiberList[ MAX_FIBERS ];

/* The index of the currently executing fiber */
static int currentFiber = -1;
/* A boolean flag indicating if we are in the main process or if we are in a fiber */
static int inFiber = 0;
/* The number of active fibers */
static int numFibers = 0;

/* The "main" execution context */
static ucontext_t mainContext;

/* Sets all the fibers to be initially inactive */
void initFibers()
{
	int i;
	for ( i = 0; i < MAX_FIBERS; ++ i )
	{
		fiberList[i].active = 0;
	}
		
	return;
}

/* Switches from a fiber to main or from main to a fiber */
void fiberYield()
{
	/* If we are in a fiber, switch to the main process */
	if ( inFiber )
	{
		/* Switch to the main context */
		LF_DEBUG_OUT1( "libfiber debug: Fiber %d yielding the processor...", currentFiber );
	
		swapcontext( &fiberList[currentFiber].context, &mainContext );
	}
	/* Else, we are in the main process and we need to dispatch a new fiber */
	else
	{
		if ( numFibers == 0 ) return;
	
		/* Saved the state so call the next fiber */
		currentFiber = (currentFiber + 1) % numFibers;
		
		LF_DEBUG_OUT1( "Switching to fiber %d.", currentFiber );
		inFiber = 1;
		swapcontext( &mainContext, &fiberList[ currentFiber ].context );
		inFiber = 0;
		LF_DEBUG_OUT1( "Fiber %d switched to main context.", currentFiber );
		
		if ( fiberList[currentFiber].active == 0 )
		{
			LF_DEBUG_OUT1( "Fiber %d is finished. Cleaning up.\n", currentFiber );
			/* Free the "current" fiber's stack */
			free( fiberList[currentFiber].stack );
			
			/* Swap the last fiber with the current, now empty, entry */
			-- numFibers;
			if ( currentFiber != numFibers )
			{
				fiberList[ currentFiber ] = fiberList[ numFibers ];
			}
			fiberList[ numFibers ].active = 0;		
		}
		
	}
	return;
}

/* Records when the fiber has started and when it is done
so that we know when to free its resources. It is called in the fiber's
context of execution. */
static void fiberStart( void (*func)(void) )
{
	fiberList[currentFiber].active = 1;
	func();
	fiberList[currentFiber].active = 0;
	
	/* Yield control, but because active == 0, this will free the fiber */
	fiberYield();
}

int spawnFiber( void (*func)(void)){
    
	if ( numFibers == MAX_FIBERS ) return LF_MAXFIBERS;
	
	/* Add the new function to the end of the fiber list */
	getcontext( &fiberList[numFibers].context );

	/* Set the context to a newly allocated stack */
	fiberList[numFibers].context.uc_link = 0;
	fiberList[numFibers].stack = malloc( FIBER_STACK );
	fiberList[numFibers].context.ss_sp = fiberList[numFibers].stack;
	fiberList[numFibers].context.ss_size = FIBER_STACK;
	fiberList[numFibers].context.ss_flags = 0;
	
	if ( fiberList[numFibers].stack == 0 )
	{
		LF_DEBUG_OUT( "Error: Could not allocate stack." );
		return LF_MALLOCERROR;
	}
	
	/* Create the context. The context calls fiberStart( func ). */
	makecontext( &fiberList[ numFibers ].context, (void (*)(void)) &fiberStart, 1, func );
	++ numFibers;
	
	return LF_NOERROR;
}

int waitForAllFibers()
{
	int fibersRemaining = 0;
	
	/* If we are in a fiber, wait for all the *other* fibers to quit */
	if ( inFiber ) fibersRemaining = 1;
	
	LF_DEBUG_OUT1( "Waiting until there are only %d threads remaining...", fibersRemaining );
	
	/* Execute the fibers until they quit */
	while ( numFibers > fibersRemaining )
	{
		fiberYield();
	}
	
	return LF_NOERROR;
}

