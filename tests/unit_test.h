#ifndef _UNIT_TEST_H_
#define _UNIT_TEST_H_

#include <stdbool.h>

#define TTY_RETURN  (1088)

extern bool TTY_OPEN_CALLED;

void run_all_tests();
void reset_fs();

#endif

