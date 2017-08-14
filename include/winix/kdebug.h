#ifndef _W_DEBUG_
#define _W_DEBUG_ 1

extern int DEBUG_SCHED;
extern int DEBUG_IPC;


#ifdef _DEBUG
    #define KDEBUG(token)   dbg_kprintf token
#else
    #define KDEBUG(token)
#endif

#endif
