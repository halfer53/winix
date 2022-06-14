#ifndef _UNIT_TEST_H_
#define _UNIT_TEST_H_

#include <stdbool.h>
#include <winix/page.h>

#define TTY_RETURN  (1088)

extern bool TTY_OPEN_CALLED;

extern const char *FILE1;
extern const char *FILE2;
extern const char *DIR_NAME;
extern const char *DIR_FILE1;
extern const char *DIR_FILE2;
extern char buffer[PAGE_LEN];
extern char buffer2[PAGE_LEN];

void run_all_tests();
void reset_fs();

#endif

