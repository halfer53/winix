#ifndef _DEBUG_H_
#define _DEBUG_H_ 1

void get_curr_addr(int *addr);

#define D_PRINT(token) printf( "%s: %d\n", #token,token)
#define X_PRINT(token) printf( "%s: 0x%x\n", #token,token)
#define S_PRINT(token) printf( "%s: %s\n", #token,token)

// #ifdef _DEBUG
//     #define XDEBUG(...) printf(__VA_ARGS__);
// #else
//     #define XDEBUG(...)
// #endif

#define WHERE_LOG printf("[LOG]%s: %d\n",__FILE__,__LINE__);


#define CS106LAssert(expr) DoCS106LAssert(expr, #expr, __FILE__, __LINE__)

#endif
