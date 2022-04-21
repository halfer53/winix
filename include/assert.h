#ifndef _ASSERT_H_
#define _ASSERT_H_

void _assert(int expression, int line, char* filename);

#define assert(expression)    _assert(expression, __LINE__ , __FILE__)

#endif
