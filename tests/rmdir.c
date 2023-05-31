#include <fs/fs.h>
#include "unit_test.h"
#include "../fs/mock/mock.h"
#include <assert.h>

void test_rmdir_file_should_fail(){
    int ret;
    ret = sys_creat(curr_scheduling_proc, "/test", 0755);
    assert(ret == 0);

    ret = sys_rmdir(curr_scheduling_proc, "/test");
    assert(ret == -ENOTDIR);
}

void test_rmdir_empty_directory_should_succeed(){
    int ret;
    ret = sys_mkdir(curr_scheduling_proc, "/test", 0755);
    assert(ret == 0);

    ret = sys_rmdir(curr_scheduling_proc, "/test");
    assert(ret == 0);
}

void test_rmdir_non_emtpy_directory_should_fail_with_enotempty(){
    int ret;
    ret = sys_mkdir(curr_scheduling_proc, "/test", 0755);
    assert(ret == 0);

    ret = sys_creat(curr_scheduling_proc, "/test/test", 0755);
    assert(ret == 0);

    ret = sys_rmdir(curr_scheduling_proc, "/test");
    assert(ret == -ENOTEMPTY);
}