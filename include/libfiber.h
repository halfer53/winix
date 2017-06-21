#ifndef LIBFIBER_H
#define LIBFIBER_H 1

#define	LF_NOERROR	0
#define	LF_MAXFIBERS	1
#define LF_MALLOCERROR	2
#define LF_CLONEERROR	3
#define	LF_INFIBER	4
#define LF_SIGNALERROR	5

/* Define a debugging output macro */
#ifdef LF_DEBUG

#include <stdio.h>
#define LF_DEBUG_OUT( string ) fprintf( stderr, "libfiber debug: " string "\n")
#define LF_DEBUG_OUT1( string, arg ) fprintf( stderr, "libfiber debug: " string "\n", arg )

#else

#define LF_DEBUG_OUT( string )
#define LF_DEBUG_OUT1( string, arg )

#endif

/* The maximum number of fibers that can be active at once. */
#define MAX_FIBERS 10
/* The size of the stack for each fiber. */
#define FIBER_STACK (1000)


/* Should be called before executing any of the other functions. */
extern void initFibers();

/* Creates a new fiber, running the function that is passed as an argument. */
extern int spawnFiber( void (*func)(void) );

/* Yield control to another execution context. */
extern void fiberYield();

/* Execute the fibers until they all quit. */
extern int waitForAllFibers();

#endif

