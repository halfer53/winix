#ifndef _DEBUG_H_
#define _DEBUG_H_ 1

#include <stdio.h>

void* get_pc();
void* get_sp();
void* get_ra();
void* fcn_to_ptr(void(*)());

#ifdef _SYSTEM
    #define D_PRINT(token) kprintf( "%s: %d\n", #token,token)
    #define X_PRINT(token) kprintf( "%s: 0x%x\n", #token,token)
    #define S_PRINT(token) kprintf( "%s: %s\n", #token,token)
#else
    #define D_PRINT(token) printf( "%s: %d\n", #token,token)
    #define X_PRINT(token) printf( "%s: 0x%x\n", #token,token)
    #define S_PRINT(token) printf( "%s: %s\n", #token,token)
#endif

#ifdef _DEBUG
//    #define XDEBUG(token) printf("%d in file %s ", __LINE__, __FILE__); printf token
    #define XDEBUG(token) printf token
#else
    #define XDEBUG(token)
#endif


#define WHERE_LOG() printf("[LOG]%s: %d\n",__FILE__,__LINE__);

#endif
