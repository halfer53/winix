#ifndef _DEBUG_H_
#define _DEBUG_H_ 1

void* get_pc();
void* get_sp();
void* fcn_to_ptr(void(*)());

#define D_PRINT(token) printf( "%s: %d\n", #token,token)
#define X_PRINT(token) printf( "%s: 0x%x\n", #token,token)
#define S_PRINT(token) printf( "%s: %s\n", #token,token)

#ifdef _DEBUG
    #define XDEBUG(token) printf token
#else
    #define XDEBUG(token)
#endif


#define WHERE_LOG() printf("[LOG]%s: %d\n",__FILE__,__LINE__);

#endif
