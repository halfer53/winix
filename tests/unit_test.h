#ifndef _UNIT_TEST_H_
#define _UNIT_TEST_H_

#include <stdbool.h>
#include <winix/comm.h>

#define TTY_RETURN  (1088)

extern bool TTY_OPEN_CALLED;

extern char buffer[PAGE_LEN];
extern char buffer2[PAGE_LEN];

void run_all_tests();
void reset_fs();

#endif

