#ifndef _ASSERT_H_
#define _ASSERT_H_

void __assert(int expression, int line, char* filename);

#define assert(expression)    __assert(expression, __LINE__ , __FILE__)

#endif
