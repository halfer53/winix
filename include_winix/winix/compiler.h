#ifndef _K_COMPILER_H_
#define _K_COMPILER_H_ 1

#include <sys/compiler.h>

void _panic(const char *str, const char* file);
void _assert(int expression, int line, char* filename);



#define ASSERT(expression)    _assert(expression, __LINE__ , __FILE__)
#define ASSERT_NOT_NULL(exp)    ASSERT(exp != NULL)
#define ASSERT_NULL(exp)        ASSERT(exp == NULL)
#define ASSERT_TRUE(exp)        ASSERT(exp == true)
#define ASSERT_FALSE(exp)       ASSERT(exp == false)
#define ASSERT_OK(exp)          ASSERT(exp == 0)
#define PANIC(expression)     _panic(expression, __FILE__)

#define READ_ONCE(v)    (v)

#endif
