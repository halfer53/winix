#ifndef _K_COMPILER_H_
#define _K_COMPILER_H_ 1

void _assert(int expression, int line, char* filename);

#define BUILD_BUG_ON(condition) extern int build_bug_on[(condition) ? 0 : 1]
#define BUILD_BUG_ON_FALSE(condition) extern int build_bug_on[(condition) ? 1 : 0]
#define BUILD_BUG_ON_ZERO(e) (sizeof(struct { int:(-!!(e)); }))
#define BUILD_BUG_ON_NULL(e) ((void *)sizeof(struct { int:(-!!(e)); }))

#define ASSERT(expression)    _assert(expression, __LINE__ , __FILE__)
#define ASSERT_NOT_NULL(exp)    ASSERT(exp != NULL)
#define ASSERT_NULL(exp)        ASSERT(exp == NULL)
#define ASSERT_TRUE(exp)        ASSERT(exp == true)
#define ASSERT_FALSE(exp)       ASSERT(exp == false)
#define ASSERT_OK(exp)          ASSERT(exp == OK)
#define PANIC(expression)     _panic(expression, __FILE__)

#define READ_ONCE(v)    (v)

#endif
