#include <fs/fs.h>
#include "unit_test.h"
#include "../fs/mock/mock.h"
#include <assert.h>
#include <signal.h>
#include <libgen.h>
#include <stdlib.h>

const char * dirent_array[] = {
        ".",
        "..",
        "bar.txt",
        "bar2.txt"
};

const char *FILE1 = "/foo.txt";
const char *FILE2 = "/foo2.txt";
const char *DIR_NAME = "/dir/";
const char *DIR_FILE1 = "/dir/bar.txt";
const char *DIR_FILE2 = "/dir/bar2.txt";
const char *TTY_PATH = "/tty";
const int TTY_DEV = MAKEDEV(3, 1);
char buffer[PAGE_LEN];
char buffer2[PAGE_LEN];

int file_size(struct proc* who, int fd){
    struct stat statbuf;
    (void)sys_fstat(who, fd, &statbuf);
    return statbuf.st_size;
}

void reset_fs(){
    init_disk();
    init_dev();
    init_fs();
    init_tty();
    init_drivers();
    mock_init_proc();
    memset(buffer, 0, PAGE_LEN);
    memset(buffer2, 0, PAGE_LEN);
}

void _close_delete_file(int fd, char *name){
    int ret;
    ret = sys_close(curr_scheduling_proc, fd);
    assert(ret == 0);

    ret = sys_unlink(curr_scheduling_proc, FILE1);
    assert(ret == 0);
}

void test_given_open_when_flag_is_o_create_should_return_0(){
    int fd;
    
    fd = sys_open(curr_scheduling_proc, FILE1 ,O_CREAT | O_RDWR, 0755);
    assert(fd == 0);

}


void test_given_open_when_openned_max_exceeds_should_return_emfile(){
    int ret;
    int i;

    for (i = 0; i < OPEN_MAX; i++){
        ret = sys_open(curr_scheduling_proc, "/", O_RDONLY, 0);
        assert(i >= 0);
    }
    ret = sys_open(curr_scheduling_proc, "/", O_RDONLY, 0);
    assert(ret == -EMFILE);

}

void test_given_open_when_openned_exceeds_system_limit_should_return_enfile(){
    int ret;
    struct proc p2, p3;
    int i;
    emulate_fork(curr_scheduling_proc, &p2);
    emulate_fork(curr_scheduling_proc, &p3);
    assert(OPEN_MAX * 2 == NR_FILPS);

    for (i = 0; i < OPEN_MAX; i++){
        ret = sys_open(curr_scheduling_proc, "/", O_RDONLY, 0);
        assert(i >= 0);
    }
    for (i = 0; i < OPEN_MAX; i++){
        ret = sys_open(&p2, "/", O_RDONLY, 0);
        assert(ret >= 0);
    }
    ret = sys_open(&p3, "/", O_RDONLY, 0);
    assert(ret == -ENFILE);

}

void test_given_open_when_flag_write_and_path_directory_should_return_eisdir(){
    int ret = sys_open(curr_scheduling_proc, "/", O_WRONLY, 0);
    assert(ret == -EISDIR);
}

void test_given_creat_when_file_not_present_should_return_0(){
    int fd;
    
    fd = sys_creat(curr_scheduling_proc, FILE1 , O_RDWR);
    assert(fd == 0);

}

void test_given_creat_when_file_present_should_return_eexist(){
    int fd;
    
    fd = sys_creat(curr_scheduling_proc, FILE1 , O_RDWR);
    assert(fd == 0);

    fd = sys_creat(curr_scheduling_proc, FILE1 , O_RDWR);
    assert(fd == -EEXIST);

}

void test_given_close_when_file_closed_should_return_ebadf(){
    int fd, ret;
    
    fd = sys_open(curr_scheduling_proc, FILE1 ,O_CREAT | O_RDWR, 0755);
    assert(fd == 0);

    _close_delete_file(fd, FILE1);

    ret = sys_close(curr_scheduling_proc, fd);
    assert(ret == -EBADF);

}

void test_given_read_when_fd_is_closed_return_ebadf(){
    int ret, fd;
    
    fd = sys_creat(curr_scheduling_proc, FILE1 , O_RDWR);
    assert(fd == 0);

    ret = sys_close(curr_scheduling_proc, fd);
    assert(ret == 0);

    ret = sys_read(curr_scheduling_proc, fd, buffer, PAGE_LEN);
    assert(ret == -EBADF);

}

void test_given_write_when_fd_is_closed_return_ebadf(){
    int ret, fd;
    
    fd = sys_creat(curr_scheduling_proc, FILE1 , O_RDWR);
    assert(fd == 0);

    ret = sys_close(curr_scheduling_proc, fd);
    assert(ret == 0);

    ret = sys_write(curr_scheduling_proc, fd, buffer, PAGE_LEN);
    assert(ret == -EBADF);

}

void test_given_open_when_deleting_file_should_return_error(){
    int ret, fd;
    
    fd = sys_creat(curr_scheduling_proc, FILE1 , O_RDWR);
    assert(fd == 0);

    _close_delete_file(fd, FILE1);

    ret = sys_open(curr_scheduling_proc, FILE1 , O_RDWR, 0775);
    assert(ret == -ENOENT);

}

void test_given_dup_when_dupping_file_should_result_in_same_fd(){
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

}

void test_given_read_when_open_and_closing_file_should_persistted_data(){
    int ret, fd;
    memset(buffer, 0xf, PAGE_LEN);

    fd = sys_open(curr_scheduling_proc, FILE1 , O_CREAT | O_RDONLY, 0x0775);
    assert(fd == 0);
    
    ret = sys_write(curr_scheduling_proc, fd, buffer, PAGE_LEN);
    assert(ret == PAGE_LEN);

    ret = sys_write(curr_scheduling_proc, fd, buffer, PAGE_LEN);
    assert(ret == PAGE_LEN);

    ret = sys_close(curr_scheduling_proc, fd);
    assert(ret == 0);

    fd = sys_open(curr_scheduling_proc, FILE1 , O_RDONLY, 0x0775);
    assert(file_size(curr_scheduling_proc, fd) == PAGE_LEN * 2);

    ret = sys_read(curr_scheduling_proc, fd, buffer2, PAGE_LEN);
    assert(ret == PAGE_LEN);
    assert(memcmp(buffer, buffer2, PAGE_LEN) == 0);

    ret = sys_read(curr_scheduling_proc, fd, buffer2, PAGE_LEN);
    assert(ret == PAGE_LEN);
    assert(memcmp(buffer, buffer2, PAGE_LEN) == 0);

    ret = sys_read(curr_scheduling_proc, fd, buffer2, PAGE_LEN);
    assert(ret == 0);

}

void test_given_read_when_o_direct_open_and_closing_file_should_persistted_data(){
    int ret, fd;
    memset(buffer, 0xf, PAGE_LEN);

    fd = sys_open(curr_scheduling_proc, FILE1 , O_CREAT | O_RDONLY | O_DIRECT, 0x0775);
    assert(fd == 0);
    
    ret = sys_write(curr_scheduling_proc, fd, buffer, PAGE_LEN);
    assert(ret == PAGE_LEN);

    ret = sys_write(curr_scheduling_proc, fd, buffer, PAGE_LEN);
    assert(ret == PAGE_LEN);

    ret = sys_close(curr_scheduling_proc, fd);
    assert(ret == 0);

    fd = sys_open(curr_scheduling_proc, FILE1 , O_RDONLY | O_DIRECT, 0x0775);
    assert(file_size(curr_scheduling_proc, fd) == PAGE_LEN * 2);

    ret = sys_read(curr_scheduling_proc, fd, buffer2, PAGE_LEN);
    assert(ret == PAGE_LEN);
    assert(memcmp(buffer, buffer2, PAGE_LEN) == 0);

    ret = sys_read(curr_scheduling_proc, fd, buffer2, PAGE_LEN);
    assert(ret == PAGE_LEN);
    assert(memcmp(buffer, buffer2, PAGE_LEN) == 0);

    ret = sys_read(curr_scheduling_proc, fd, buffer2, PAGE_LEN);
    assert(ret == 0);

}


void test_given_access_when_file_not_exist_should_return_enoent(){
    int ret = sys_access(curr_scheduling_proc, FILE1, F_OK);
    assert(ret == -ENOENT);

}

void test_given_access_when_file_exists_should_return_0(){
    int ret;
    ret = sys_creat(curr_scheduling_proc, FILE1, 0755);
    assert(ret == 0);

    ret = sys_access(curr_scheduling_proc, FILE1, F_OK);
    assert(ret == 0);

}

void test_given_access_when_folder_exists_should_return_0(){
    int ret;
    ret = sys_mkdir(curr_scheduling_proc, DIR_NAME, 0755);
    assert(ret == 0);

    ret = sys_access(curr_scheduling_proc, DIR_NAME, F_OK);
    assert(ret == 0);

}

void test_given_access_when_under_folder_should_return_enoent(){
    int ret;
    ret = sys_mkdir(curr_scheduling_proc, DIR_NAME, 0755);
    assert(ret == 0);

    ret = sys_access(curr_scheduling_proc, DIR_FILE1, F_OK);
    assert(ret == -ENOENT);

}

void test_given_link_stat_when_two_files_are_linked_should_return_same(){
    int fd, ret;
    struct stat statbuf, statbuf2;

    fd = sys_creat(curr_scheduling_proc, FILE1, O_RDWR);
    assert(fd == 0);

    ret = sys_link(curr_scheduling_proc, FILE1, FILE2);
    assert(ret == 0);

    ret = sys_stat(curr_scheduling_proc, FILE1, &statbuf);
    assert(ret == 0);

    ret = sys_stat(curr_scheduling_proc, FILE2, &statbuf2);
    assert(ret == 0);
    assert(statbuf.st_ino == statbuf2.st_ino);
    assert(statbuf.st_dev == statbuf2.st_dev);
    assert(statbuf.st_nlink == 2);

}


void test_given_link_stat_when_one_file_deleted_should_return_1_nlink(){
    int fd, ret;
    struct stat statbuf;

    fd = sys_creat(curr_scheduling_proc, FILE1, O_RDWR);
    assert(fd == 0);

    ret = sys_link(curr_scheduling_proc, FILE1, FILE2);
    assert(ret == 0);

    ret = sys_stat(curr_scheduling_proc, FILE1, &statbuf);
    assert(ret == 0);
    assert(statbuf.st_nlink == 2);

    ret = sys_unlink(curr_scheduling_proc, FILE1);
    assert(ret == 0);

    ret = sys_stat(curr_scheduling_proc, FILE2, &statbuf);
    assert(ret == 0);
    assert(statbuf.st_nlink == 1);

}

void test_given_chdir_when_dir_not_present_should_return_eexist(){
    int ret = sys_chdir(curr_scheduling_proc, "/not_exist");
    assert(ret == -EEXIST);
}

void test_given_chdir_when_path_is_file_should_return_eexist(){
    int fd = sys_creat(curr_scheduling_proc, FILE1, O_RDWR);
    assert(fd == 0);

    int ret = sys_chdir(curr_scheduling_proc, FILE1);
    assert(ret == -ENOTDIR);

}

void test_given_chdir_when_dir_is_valid_should_succeed(){
    struct stat statbuf;
    int ret = sys_mkdir(curr_scheduling_proc, DIR_NAME, O_RDWR);
    assert(ret == 0);

    ret = sys_chdir(curr_scheduling_proc, DIR_NAME);
    assert(ret == 0);

    ret = sys_stat(curr_scheduling_proc, DIR_NAME, &statbuf);
    assert(ret == 0);
    assert(curr_scheduling_proc->fp_workdir->i_num == statbuf.st_ino);

}

void test_given_chmod_when_file_not_present_should_return_enonent(){
    int ret = sys_chmod(curr_scheduling_proc, "/notexists", O_RDONLY);
    assert(ret == -ENOENT);
}

void test_given_chmod_stat_when_file_present_should_return_0(){
    int fd, ret;
    struct stat statbuf;

    fd = sys_creat(curr_scheduling_proc, FILE1, O_RDWR);
    assert(fd == 0);

    ret = sys_chmod(curr_scheduling_proc, FILE1, 0777);
    assert(ret == 0);

    ret = sys_stat(curr_scheduling_proc, FILE1, &statbuf);
    assert(ret == 0);
    assert(statbuf.st_mode == 0777);

}

void test_given_chmod_stat_when_folder_present_should_return_0(){
    int fd, ret;
    struct stat statbuf;

    fd = sys_mkdir(curr_scheduling_proc, DIR_NAME, O_RDWR);
    assert(fd == 0);

    ret = sys_chmod(curr_scheduling_proc, DIR_NAME, 0777);
    assert(ret == 0);

    ret = sys_stat(curr_scheduling_proc, DIR_NAME, &statbuf);
    assert(ret == 0);
    assert(statbuf.st_mode == 0777);

}

void test_given_getdents_when_no_files_in_folder_should_return_default_files(){
    struct dirent dir[5];
    int i;
    int fd = sys_open(curr_scheduling_proc, "/", O_RDONLY, 0);
    int ret = sys_getdents(curr_scheduling_proc, fd, dir, 5);
    assert(ret == sizeof(struct dirent) * 2);
    for(i = 0; i < 2; i++){
        assert(char32_strcmp(dir[i].d_name, dirent_array[i]) == 0);
    }

    ret = sys_close(curr_scheduling_proc, fd);
    assert(ret == 0);
}

void test_given_getdents_when_files_in_folder_should_return_files(){
    int ret, fd, fd2, i;
    struct dirent dir[5];

    ret = sys_mkdir(curr_scheduling_proc, DIR_NAME, O_RDWR);
    assert(ret == 0);

    fd = sys_creat(curr_scheduling_proc, DIR_FILE1, O_RDWR);
    assert(fd == 0);

    ret = sys_link(curr_scheduling_proc, DIR_FILE1, DIR_FILE2);
    assert(ret == 0);

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

    assert(sys_close(curr_scheduling_proc, fd) == 0);
    assert(sys_close(curr_scheduling_proc, fd2) == 0);
}


void test_given_getdents_when_successive_call_should_return_files(){
    int ret, fd, fd2;
    struct dirent dir[5];

    ret = sys_mkdir(curr_scheduling_proc, DIR_NAME, O_RDWR);
    assert(ret == 0);

    fd = sys_creat(curr_scheduling_proc, DIR_FILE1, O_RDWR);
    assert(fd == 0);

    ret = sys_link(curr_scheduling_proc, DIR_FILE1, DIR_FILE2);
    assert(ret == 0);

    fd2 = sys_open(curr_scheduling_proc, DIR_NAME, O_RDONLY, 0);
    assert(fd2 == 1);

    ret = sys_getdents(curr_scheduling_proc, fd2, dir, 1);
    assert(ret == sizeof(struct dirent));
    assert(char32_strcmp(dir[0].d_name, ".") == 0);

    ret = sys_getdents(curr_scheduling_proc, fd2, dir, 1);
    assert(ret == sizeof(struct dirent));
    assert(char32_strcmp(dir[0].d_name, "..") == 0);

    ret = sys_getdents(curr_scheduling_proc, fd2, dir, 1);
    assert(ret == sizeof(struct dirent));
    assert(char32_strcmp(dir[0].d_name, basename(DIR_FILE1)) == 0);

    ret = sys_getdents(curr_scheduling_proc, fd2, dir, 1);
    assert(ret == sizeof(struct dirent));
    assert(char32_strcmp(dir[0].d_name, basename(DIR_FILE2)) == 0);

    ret = sys_getdents(curr_scheduling_proc, fd2, dir, 5);
    assert(ret == 0);

    assert(sys_close(curr_scheduling_proc, fd) == 0);
    assert(sys_close(curr_scheduling_proc, fd2) == 0);
}

void test_given_cwd_when_chdir_should_return_path(){
    int ret;
    char *result;
    ret = sys_mkdir(curr_scheduling_proc, DIR_NAME, 0755);
    assert(ret == 0);

    ret = sys_chdir(curr_scheduling_proc, DIR_NAME);
    assert(ret == 0);

    ret = sys_getcwd(curr_scheduling_proc, buffer, PAGE_LEN, &result);
    assert(ret == 0);
    assert(strcmp(result, DIR_NAME) == 0);
}

void test_given_cwd_when_path_is_1_should_return_einvalid(){
    char *result;
    int ret = sys_getcwd(curr_scheduling_proc, buffer, 1, &result);
    assert(ret == -ERANGE);
}


void test_given_mknod_when_path_valid_should_return_0(){
    int ret = sys_mknod(curr_scheduling_proc, TTY_PATH, O_RDWR, TTY_DEV);
    assert(ret == 0);

}

void test_given_dev_open_when_file_is_driver_should_return_from_driver(){
    int ret = sys_mknod(curr_scheduling_proc, TTY_PATH, O_RDWR, TTY_DEV);
    assert(ret == 0);

    TTY_OPEN_CALLED = false;

    int fd = sys_open(curr_scheduling_proc, TTY_PATH, O_EXCL | O_RDWR, 0);
    assert(fd == 0);
    assert(TTY_OPEN_CALLED == true);

}

void test_given_dev_read_when_file_is_driver_should_return_from_driver(){
    int ret = sys_mknod(curr_scheduling_proc, TTY_PATH, O_RDWR, TTY_DEV);
    assert(ret == 0);

    int fd = sys_open(curr_scheduling_proc, TTY_PATH, O_EXCL | O_RDWR, 0);
    assert(fd == 0);

    ret = sys_read(curr_scheduling_proc, fd, buffer, 3);
    assert(ret == TTY_RETURN);

}

void test_given_dev_write_when_file_is_driver_should_return_from_driver(){
    int ret = sys_mknod(curr_scheduling_proc, TTY_PATH, O_RDWR, TTY_DEV);
    assert(ret == 0);

    int fd = sys_open(curr_scheduling_proc, TTY_PATH, O_EXCL | O_RDWR, 0);
    assert(fd == 0);

    ret = sys_write(curr_scheduling_proc, fd, buffer, 3);
    assert(ret == TTY_RETURN);

}

void test_given_dev_close_when_file_is_driver_should_return_from_driver(){
    int ret = sys_mknod(curr_scheduling_proc, TTY_PATH, O_RDWR, TTY_DEV);
    assert(ret == 0);

    int fd = sys_open(curr_scheduling_proc, TTY_PATH, O_EXCL | O_RDWR, 0);
    assert(fd == 0);

    ret = sys_close(curr_scheduling_proc, fd);
    assert(ret == TTY_RETURN);

}

void test_given_dev_dup_when_file_is_driver_should_return_from_driver(){
    int ret = sys_mknod(curr_scheduling_proc, TTY_PATH, O_RDWR, TTY_DEV);
    assert(ret == 0);

    int fd = sys_open(curr_scheduling_proc, TTY_PATH, O_EXCL | O_RDWR, 0);
    assert(fd == 0);

    int fd2 = sys_dup2(curr_scheduling_proc, fd, 1);
    assert(1 == fd2);

    ret = sys_write(curr_scheduling_proc, fd, buffer, 3);
    assert(ret == TTY_RETURN);

    ret = sys_write(curr_scheduling_proc, fd2, buffer, 3);
    assert(ret == TTY_RETURN);

    ret = sys_read(curr_scheduling_proc, fd, buffer, 3);
    assert(ret == TTY_RETURN);

    ret = sys_read(curr_scheduling_proc, fd2, buffer, 3);
    assert(ret == TTY_RETURN);

    ret = sys_close(curr_scheduling_proc, fd);
    assert(ret == TTY_RETURN);

    ret = sys_close(curr_scheduling_proc, fd2);
    assert(ret == TTY_RETURN);
}

void test_when_zone_full_should_return_enospc(){
    int fd = sys_open(curr_scheduling_proc, FILE1, O_CREAT | O_RDWR, 0755);
    assert(fd == 0);

    int remaining_bytes = MAX_ZONES * BLOCK_SIZE;
    char *_buffer = malloc(remaining_bytes);
    int ret = sys_write(curr_scheduling_proc, fd, _buffer, remaining_bytes);
    assert(ret == remaining_bytes);

    ret = sys_write(curr_scheduling_proc, fd, _buffer, 1);
    assert(ret == -EFBIG);

    free(_buffer);
}

void test_given_directory_when_read_should_return_eisdir(){
    int ret = sys_mkdir(curr_scheduling_proc, DIR_NAME, 0755);
    assert(ret == 0);

    int fd = sys_open(curr_scheduling_proc, DIR_NAME, O_RDONLY, 0);
    assert(fd == 0);

    ret = sys_read(curr_scheduling_proc, fd, buffer, 3);
    assert(ret == -EISDIR);

    ret = sys_close(curr_scheduling_proc, fd);
    assert(ret == 0);
}
