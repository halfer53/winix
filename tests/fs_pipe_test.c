#include <fs/fs.h>
#include "unit_test.h"
#include "../fs/mock/mock.h"
#include <assert.h>
#include <signal.h>

void _init_pipe(int pipe_fd[2], struct proc* pcurr2){
    int ret;
    pcurr2->pid = 2;
    pcurr2->proc_nr = 2;

    ret = sys_pipe(curr_scheduling_proc, pipe_fd);
    assert(ret == 0);

    emulate_fork(curr_scheduling_proc, pcurr2);
}

void __close_pipe(int pipe_fd[2], struct proc* process, int num){
    sys_close(process, pipe_fd[num]);
    sys_close(process, pipe_fd[num]);
}

void _close_pipe(int pipe_fd[2], struct proc* pcurr2){
    __close_pipe(pipe_fd, curr_scheduling_proc, 0);
    __close_pipe(pipe_fd, pcurr2, 1);
}

void test_given_pipe_read_when_no_data_in_pipe_should_return_suspend(){

    struct proc pcurr2;
    int ret;
    int pipe_fd[2];

    _init_pipe(pipe_fd, &pcurr2);

    ret = sys_read(curr_scheduling_proc, pipe_fd[0], buffer, 100);
    assert(ret == SUSPEND);

}

void test_given_pipe_write_when_no_data_in_pipe_should_return_succeed(){

    struct proc pcurr2;
    int ret;
    int pipe_fd[2];

    _init_pipe(pipe_fd, &pcurr2);

    ret = sys_write(curr_scheduling_proc, pipe_fd[1], "a", 1);
    assert(ret == 1);

}

void test_given_pipe_write_when_pipe_is_full_should_return_suspend(){

    struct proc pcurr2;
    int ret;
    int pipe_fd[2];

    _init_pipe(pipe_fd, &pcurr2);

    ret = sys_write(curr_scheduling_proc, pipe_fd[1], buffer, PAGE_LEN);
    assert(ret == PAGE_LEN);

    ret = sys_write(curr_scheduling_proc, pipe_fd[1], buffer, PAGE_LEN);
    assert(ret == SUSPEND);

}

void test_given_pipe_read_when_proc_was_suspended_should_return(){

    struct proc pcurr2;
    int ret;
    int pipe_fd[2];

    _init_pipe(pipe_fd, &pcurr2);
    memset(buffer, 0, PAGE_LEN);

    ret = sys_read(curr_scheduling_proc, pipe_fd[0], buffer, 100);
    assert(ret == SUSPEND);
    assert(*buffer == 0);

    ret = sys_write(&pcurr2, pipe_fd[1], "1234", 5);
    assert(ret == 5);
    assert(*buffer == 0);

    ret = sys_close(curr_scheduling_proc, pipe_fd[1]);
    ret = sys_close(&pcurr2, pipe_fd[1]);
    assert(ret == 0);
    assert(strcmp(buffer, "1234") == 0);
}

void test_given_pipe_read_when_data_is_written_should_return_data(){

    struct proc pcurr2;
    int ret;
    int pipe_fd[2];

    _init_pipe(pipe_fd, &pcurr2);

    ret = sys_write(&pcurr2, pipe_fd[1], "5678", 5);
    assert(ret == 5);

    ret = sys_read(curr_scheduling_proc, pipe_fd[0], buffer, 100);
    assert(ret == 5);
    assert(strcmp(buffer, "5678") == 0);

}

void test_given_pipe_read_when_pipe_is_full_should_return_data(){

    struct proc pcurr2;
    int ret;
    int pipe_fd[2];

    _init_pipe(pipe_fd, &pcurr2);

    memset(buffer, 'a', PAGE_LEN - 1);
    buffer[PAGE_LEN - 1] = 0;

    ret = sys_write(&pcurr2, pipe_fd[1], buffer, PAGE_LEN);
    assert(ret == PAGE_LEN);

    ret = sys_write(&pcurr2, pipe_fd[1], "abc", 4);
    assert(ret == SUSPEND);

    ret = sys_read(curr_scheduling_proc, pipe_fd[0], buffer2, PAGE_LEN);
    assert(ret == PAGE_LEN);
    assert(strcmp(buffer, buffer2) == 0);

    ret = sys_read(curr_scheduling_proc, pipe_fd[0], buffer2, PAGE_LEN);
    assert(ret == 4);
    assert(strcmp(buffer2, "abc") == 0);

}

void test_given_pipe_read_when_writer_write_fd_closed_should_return_suspend(){

    struct proc pcurr2;
    int ret;
    int pipe_fd[2];

    _init_pipe(pipe_fd, &pcurr2);
    memset(buffer, 0, PAGE_LEN);

    ret = sys_close(curr_scheduling_proc, pipe_fd[1]);
    assert(ret == 0);

    ret = sys_read(&pcurr2, pipe_fd[0], buffer, 4);
    assert(ret == SUSPEND);

    ret = sys_write(&pcurr2, pipe_fd[1], "abc", 4);
    assert(ret == 4);
    ret = sys_close(&pcurr2, pipe_fd[1]);
    assert(ret == 0);
    assert(strcmp(buffer, "abc") == 0);

}


void test_given_pipe_read_when_all_write_fds_closed_should_return_0(){

    struct proc pcurr2;
    int ret;
    int pipe_fd[2];

    _init_pipe(pipe_fd, &pcurr2);
    memset(buffer, 0, PAGE_LEN);

    ret = sys_close(curr_scheduling_proc, pipe_fd[1]);
    assert(ret == 0);
    
    ret = sys_close(&pcurr2, pipe_fd[1]);
    assert(ret == 0);

    ret = sys_read(&pcurr2, pipe_fd[0], buffer, 2);
    assert(ret == 0);

}

void test_given_pipe_write_when_one_read_fd_s_closed_should_return_success(){

    struct proc pcurr2;
    int ret;
    int pipe_fd[2];

    _init_pipe(pipe_fd, &pcurr2);
    memset(buffer, 0, PAGE_LEN);

    ret = sys_close(curr_scheduling_proc, pipe_fd[0]);
    assert(ret == 0);

    ret = sys_write(&pcurr2, pipe_fd[1], "a", 2);
    assert(ret == 2);

    ret = sys_read(&pcurr2, pipe_fd[0], buffer, 2);
    assert(ret == 2);
    assert(strcmp("a", buffer) == 0);

}

void test_given_pipe_write_when_read_fd_are_closed_should_return_sigpipe(){

    struct proc pcurr2;
    int ret;
    int pipe_fd[2];

    _init_pipe(pipe_fd, &pcurr2);

    ret = sys_close(curr_scheduling_proc, pipe_fd[0]);
    assert(ret == 0);

    ret = sys_close(&pcurr2, pipe_fd[0]);
    assert(ret == 0);
    
    ret = sys_write(&pcurr2, pipe_fd[1], "a", 2);
    assert(ret == SUSPEND);
    assert(sigismember(&pcurr2.sig_pending, SIGPIPE));

}

void test_given_pipe_write_when_read_fd_closed_and_sigpipe_ignored_should_return_epipe(){

    struct proc pcurr2;
    int ret;
    int pipe_fd[2];

    _init_pipe(pipe_fd, &pcurr2);

    ret = sys_close(curr_scheduling_proc, pipe_fd[0]);
    assert(ret == 0);

    ret = sys_close(&pcurr2, pipe_fd[0]);
    assert(ret == 0);
    
    pcurr2.sig_table[SIGPIPE].sa_handler = SIG_IGN;
    ret = sys_write(&pcurr2, pipe_fd[1], "a", 2);
    assert(ret == -EPIPE);

}