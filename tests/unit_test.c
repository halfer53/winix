//
// Created by bruce on 9/05/20.
//
#include "../fs/fs.h"
#include <assert.h>

const char * dirent_array[] = {
        ".",
        "..",
        "bar.txt",
        "bar2.txt"
};

const char *FILE1 = "/foo.txt";
const char *DIR_NAME = "/dev/";
const char *DIR_FILE1 = "/dev/bar.txt";
const char *DIR_FILE2 = "/dev/bar2.txt";
char buffer[PAGE_LEN];
char buffer2[PAGE_LEN];

int file_size(struct proc* who, int fd){
    struct stat statbuf;
    (void)sys_fstat(who, fd, &statbuf);
    return statbuf.st_size;
}

void _reset_fs(){
    init_disk();
    init_dev();
    init_fs();
    init_tty();
    init_drivers();
    mock_init_proc();
}

void _close_delete_file(int fd, char *name){
    int ret;
    ret = sys_close(curr_scheduling_proc, fd);
    assert(ret == 0);

    ret = sys_unlink(curr_scheduling_proc, FILE1);
    assert(ret == 0);
}

void test_given_o_creat_when_open_file_should_return_0(){
    int fd;
    
    fd = sys_open(curr_scheduling_proc, FILE1 ,O_CREAT | O_RDWR, 0775);
    assert(fd == 0);

    _close_delete_file(fd, FILE1);
}

void test_given_close_when_file_closed_should_return_ebadf(){
    int fd, ret;
    
    fd = sys_open(curr_scheduling_proc, FILE1 ,O_CREAT | O_RDWR, 0775);
    assert(fd == 0);

    _close_delete_file(fd, FILE1);

    ret = sys_close(curr_scheduling_proc, fd);
    assert(ret == EBADF);
}

void test_when_creating_file_should_return_0(){
    int fd;
    
    fd = sys_creat(curr_scheduling_proc, FILE1 , O_RDWR);
    assert(fd == 0);

    _close_delete_file(fd, FILE1);
}

void test_given_read_when_fd_is_closed_return_ebadf(){
    int ret, fd;
    
    fd = sys_creat(curr_scheduling_proc, FILE1 , O_RDWR);
    assert(fd == 0);

    ret = sys_close(curr_scheduling_proc, fd);
    assert(ret == 0);

    ret = sys_read(curr_scheduling_proc, fd, buffer, PAGE_LEN);
    assert(ret == EBADF);

    ret = sys_unlink(curr_scheduling_proc, FILE1);
    assert(ret == 0);
}

void test_given_write_when_fd_is_closed_return_ebadf(){
    int ret, fd;
    
    fd = sys_creat(curr_scheduling_proc, FILE1 , O_RDWR);
    assert(fd == 0);

    ret = sys_close(curr_scheduling_proc, fd);
    assert(ret == 0);

    ret = sys_write(curr_scheduling_proc, fd, buffer, PAGE_LEN);
    assert(ret == EBADF);

    ret = sys_unlink(curr_scheduling_proc, FILE1);
    assert(ret == 0);
}

void test_given_opening_file_when_deleting_file_should_return_error(){
    int ret, fd;
    
    fd = sys_creat(curr_scheduling_proc, FILE1 , O_RDWR);
    assert(fd == 0);

    _close_delete_file(fd, FILE1);

    ret = sys_open(curr_scheduling_proc, FILE1 , O_RDWR, 0775);
    assert(ret == ENOENT);
}

void test_given_two_file_descriptors_when_dupping_file_should_behave_the_same(){
    int ret, fd, fd2;
    
    fd = sys_open(curr_scheduling_proc, FILE1 ,O_CREAT | O_RDWR, 0775);
    assert(fd == 0);
    
    fd2 = sys_dup(curr_scheduling_proc, fd);
    assert(fd2 == fd + 1);

    ret = sys_write(curr_scheduling_proc, fd, "abc", 3);
    assert(ret == 3);

    ret = sys_write(curr_scheduling_proc, fd, "def", 4);
    assert(ret == 4);

    ret = sys_read(curr_scheduling_proc, fd, buffer, 100);
    assert(ret == 0);

    ret = sys_read(curr_scheduling_proc, fd2, buffer, 100);
    assert(ret == 0);

    ret = sys_lseek(curr_scheduling_proc, fd2, 0, SEEK_SET);
    assert(ret == 0);

    ret = sys_read(curr_scheduling_proc, fd, buffer, 100);
    assert(ret == 7);
    assert(strcmp(buffer, "abcdef") == 0);

    ret = sys_lseek(curr_scheduling_proc, fd, 0, SEEK_SET);
    assert(ret == 0);

    ret = sys_read(curr_scheduling_proc, fd2, buffer, 100);
    assert(ret == 7);
    assert(strcmp(buffer, "abcdef") == 0);

    ret = sys_close(curr_scheduling_proc, fd);
    assert(ret == 0);

    ret = sys_close(curr_scheduling_proc, fd2);
    assert(ret == 0);

    ret = sys_unlink(curr_scheduling_proc, FILE1);
    assert(ret == 0);
}

void test_given_file_data_when_open_and_closing_file_should_persist(){
    int ret, fd;

    fd = sys_open(curr_scheduling_proc, FILE1 , O_CREAT | O_RDONLY, 0x0775);
    assert(fd == 0);
    
    ret = sys_write(curr_scheduling_proc, fd, "abcdef", 7);
    assert(ret == 7);

    ret = sys_close(curr_scheduling_proc, fd);
    assert(ret == 0);

    fd = sys_open(curr_scheduling_proc, FILE1 , O_RDONLY, 0x0775);
    assert(file_size(curr_scheduling_proc, fd) == 7);

    ret = sys_read(curr_scheduling_proc, fd, buffer, 100);
    assert(ret == 7);
    assert(strcmp(buffer, "abcdef") == 0);

    ret = sys_close(curr_scheduling_proc, fd);
    assert(ret == 0);

    ret = sys_unlink(curr_scheduling_proc, FILE1);
    assert(ret == 0);
}

void _init_pipe(int pipe_fd[2], struct proc* pcurr2){
    int ret;
    pcurr2->pid = 2;
    pcurr2->proc_nr = 2;

    ret = sys_pipe(curr_scheduling_proc, pipe_fd);
    assert(ret == 0);

    emulate_fork(curr_scheduling_proc, pcurr2);
}

void __close_pipe(int pipe_fd[2], struct proc* process){
    sys_close(process, pipe_fd[0]);
    sys_close(process, pipe_fd[1]);
}

void _close_pipe(int pipe_fd[2], struct proc* pcurr2){
    __close_pipe(pipe_fd, curr_scheduling_proc);
    __close_pipe(pipe_fd, pcurr2);
}

void test_given_read_when_no_data_in_pipe_should_return_suspend(){
    struct proc pcurr2;
    int ret;
    int pipe_fd[2];

    _init_pipe(pipe_fd, &pcurr2);

    ret = sys_read(curr_scheduling_proc, pipe_fd[0], buffer, 100);
    assert(ret == SUSPEND);

    _close_pipe(pipe_fd, &pcurr2);
}

void test_given_write_when_no_data_in_pipe_should_return_succeed(){
    struct proc pcurr2;
    int ret;
    int pipe_fd[2];

    _init_pipe(pipe_fd, &pcurr2);

    ret = sys_write(curr_scheduling_proc, pipe_fd[1], "a", 1);
    assert(ret == 1);

    _close_pipe(pipe_fd, &pcurr2);
}

void test_given_write_when_pipe_is_full_should_return_suspend(){
    struct proc pcurr2;
    int ret;
    int pipe_fd[2];

    _init_pipe(pipe_fd, &pcurr2);

    ret = sys_write(curr_scheduling_proc, pipe_fd[1], buffer, PAGE_LEN);
    assert(ret == PAGE_LEN);

    ret = sys_write(curr_scheduling_proc, pipe_fd[1], buffer, PAGE_LEN);
    assert(ret == SUSPEND);

    _close_pipe(pipe_fd, &pcurr2);
}

void test_given_read_when_proc_was_suspended_should_return(){
    struct proc pcurr2;
    int ret;
    int pipe_fd[2];

    _init_pipe(pipe_fd, &pcurr2);

    ret = sys_read(curr_scheduling_proc, pipe_fd[0], buffer, 100);
    assert(ret == SUSPEND);

    ret = sys_write(&pcurr2, pipe_fd[1], "1234", 5);
    assert(ret == 5);
    assert(strcmp(buffer, "1234") == 0);

    ret = sys_write(&pcurr2, pipe_fd[1], "5678", 5);
    assert(ret == 5);

    _close_pipe(pipe_fd, &pcurr2);
}

void test_given_read_when_data_is_written_should_return_data(){
    struct proc pcurr2;
    int ret;
    int pipe_fd[2];

    _init_pipe(pipe_fd, &pcurr2);

    ret = sys_write(&pcurr2, pipe_fd[1], "5678", 5);
    assert(ret == 5);

    ret = sys_read(curr_scheduling_proc, pipe_fd[0], buffer, 100);
    assert(ret == 5);
    assert(strcmp(buffer, "5678") == 0);

    _close_pipe(pipe_fd, &pcurr2);
}

void test_given_read_when_pipe_is_full_should_return_data(){
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

    _close_pipe(pipe_fd, &pcurr2);
}

void test_given_write_when_one_read_fd_s_closed_should_return_success(){
    struct proc pcurr2;
    int ret;
    int pipe_fd[2];

    _init_pipe(pipe_fd, &pcurr2);

    ret = sys_close(curr_scheduling_proc, pipe_fd[0]);
    assert(ret == 0);

    ret = sys_write(&pcurr2, pipe_fd[1], "a", 2);
    assert(ret == 2);

    ret = sys_read(&pcurr2, pipe_fd[0], buffer, 2);
    assert(ret == 2);
    assert(strcmp("a", buffer) == 0);

    _close_pipe(pipe_fd, &pcurr2);
}

void test_given_write_when_read_fd_are_closed_should_return_sigpipe(){
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
    assert(pcurr2.sig_pending & (1 << SIGPIPE));

    _close_pipe(pipe_fd, &pcurr2);
}

void test_given_write_when_read_fd_closed_and_sigpipe_ignored_should_return_epipe(){
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
    assert(ret == EPIPE);

    _close_pipe(pipe_fd, &pcurr2);
}

void test_given_access_when_file_not_exist_should_return_enoent(){
    int ret = sys_access(curr_scheduling_proc, FILE1, F_OK);
    assert(ret == ENOENT);
}

void test_given_access_when_file_exists_should_return_0(){
    int ret;
    ret = sys_creat(curr_scheduling_proc, FILE1, 0x755);
    assert(ret == 0);

    ret = sys_access(curr_scheduling_proc, FILE1, F_OK);
    assert(ret == 0);

    _close_delete_file(0, FILE1);
}

void test_given_access_when_folder_exists_should_return_0(){
    int ret;
    ret = sys_mkdir(curr_scheduling_proc, DIR_NAME, 0x755);
    assert(ret == 0);

    ret = sys_access(curr_scheduling_proc, DIR_NAME, F_OK);
    assert(ret == 0);

    _reset_fs();
}

void test_given_access_when_under_folder_should_return_enoent(){
    int ret;
    ret = sys_mkdir(curr_scheduling_proc, DIR_NAME, 0x755);
    assert(ret == 0);

    ret = sys_access(curr_scheduling_proc, DIR_FILE1, F_OK);
    assert(ret == ENOENT);

    _reset_fs();
}

int unit_test3(){
    int ret, fd, fd2, i;
    struct stat statbuf, statbuf2;
    struct dirent dir[5];
    _reset_fs();

    ret = sys_mkdir(curr_scheduling_proc, DIR_NAME, 0x755);
    assert(ret == 0);

    fd = sys_creat(curr_scheduling_proc, DIR_FILE1, 0x777);
    assert(fd == 0);

    ret = sys_chdir(curr_scheduling_proc, DIR_NAME);
    assert(ret == 0);

    ret = sys_stat(curr_scheduling_proc, DIR_NAME, &statbuf);
    assert(ret == 0);
    assert(curr_scheduling_proc->fp_workdir->i_num == statbuf.st_ino);

    ret = sys_link(curr_scheduling_proc, DIR_FILE1, DIR_FILE2);
    assert(ret == 0);

    ret = sys_chmod(curr_scheduling_proc, DIR_FILE1, 0x777);
    assert(ret == 0);

    ret = sys_stat(curr_scheduling_proc, DIR_FILE1, &statbuf);
    assert(ret == 0);

    ret = sys_stat(curr_scheduling_proc, DIR_FILE2, &statbuf2);
    assert(ret == 0);
    assert(statbuf.st_ino == statbuf2.st_ino);
    assert(statbuf.st_dev == statbuf2.st_dev);
    assert(statbuf.st_nlink == 2);
    assert(statbuf.st_mode == 0x777);

    fd2 = sys_open(curr_scheduling_proc, DIR_NAME, O_RDONLY, 0);
    assert(fd2 == 1);

    ret = sys_getdents(curr_scheduling_proc, fd2, dir, 5);
    size_t dirent_size = sizeof(struct dirent);
    size_t desired = dirent_size * 4;
    assert(ret == desired);
    for (i = 0; i < 4; ++i) {
        assert(char32_strcmp(dir[i].d_name, dirent_array[i]) == 0);
    }
    ret = sys_getdents(curr_scheduling_proc, fd2, dir, 10);
    assert(ret == 0);

    ret = sys_unlink(curr_scheduling_proc, DIR_FILE2);
    assert(ret == 0);

    ret = sys_stat(curr_scheduling_proc, DIR_FILE1, &statbuf);
    assert(ret == 0);
    assert(statbuf.st_nlink == 1);

    ret = sys_stat(curr_scheduling_proc, DIR_FILE2, &statbuf2);
    assert(ret == ENOENT);

    ret = sys_unlink(curr_scheduling_proc, DIR_FILE1);
    assert(ret == 0);

    ret = sys_access(curr_scheduling_proc, DIR_FILE1, F_OK);
    assert(ret == ENOENT);

    ret = sys_close(curr_scheduling_proc, fd);
    assert(ret == 0);

    ret = sys_close(curr_scheduling_proc, fd2);
    assert(ret == 0);
    
    return 0;
}

int unit_test_driver(){
    int ret, fd, fd2, fd3;

    ret = sys_mknod(curr_scheduling_proc, "/dev/tty", O_RDWR, MAKEDEV(3, 1));
    assert(ret == 0);

    fd = sys_open(curr_scheduling_proc, "/dev/tty", O_RDWR, 0);
    assert(fd == 0);

    ret = sys_read(curr_scheduling_proc, fd, buffer, 3);
    assert(ret == 3);
    assert(strcmp(buffer, "tt") == 0);

    fd2 = sys_dup2(curr_scheduling_proc, fd, 1);
    assert(1 == fd2);

    ret = sys_read(curr_scheduling_proc, fd2, buffer, 4);
    assert(ret == 4);
    assert(strcmp(buffer, "ttt") == 0);

    ret = sys_close(curr_scheduling_proc, fd);
    assert(ret == 0);

    ret = sys_close(curr_scheduling_proc, fd2);
    assert(ret == 0);

    ret = sys_close(curr_scheduling_proc, fd3);
    assert(ret == EBADF);
}

int main(){

    init_bitmap();
    _reset_fs();

    test_given_o_creat_when_open_file_should_return_0();
    test_when_creating_file_should_return_0();
    test_given_opening_file_when_deleting_file_should_return_error();
    test_given_two_file_descriptors_when_dupping_file_should_behave_the_same();
    test_given_file_data_when_open_and_closing_file_should_persist();
    test_given_read_when_no_data_in_pipe_should_return_suspend();
    test_given_write_when_no_data_in_pipe_should_return_succeed();
    test_given_write_when_pipe_is_full_should_return_suspend();
    test_given_read_when_fd_is_closed_return_ebadf();
    test_given_write_when_fd_is_closed_return_ebadf();
    test_given_close_when_file_closed_should_return_ebadf();
    test_given_read_when_proc_was_suspended_should_return();
    test_given_read_when_data_is_written_should_return_data();
    test_given_read_when_pipe_is_full_should_return_data();
    test_given_write_when_read_fd_are_closed_should_return_sigpipe();
    test_given_write_when_one_read_fd_s_closed_should_return_success();
    test_given_write_when_read_fd_closed_and_sigpipe_ignored_should_return_epipe();
    test_given_access_when_file_not_exist_should_return_enoent();
    test_given_access_when_file_exists_should_return_0();
    test_given_access_when_folder_exists_should_return_0();
    test_given_access_when_under_folder_should_return_enoent();
    
    unit_test3();
    unit_test3();
    unit_test_driver();
    printf("filesystem unit test passed\n");
    return 0;
}

