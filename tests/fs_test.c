#include <fs/fs.h>
#include "unit_test.h"
#include "../fs/mock/mock.h"
#include <assert.h>
#include <signal.h>

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
struct proc* current;

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
    ret = sys_close(current, fd);
    assert(ret == 0);

    ret = sys_unlink(current, FILE1);
    assert(ret == 0);
}

void test_given_open_when_flag_is_o_create_should_return_0(){
    int fd;
    
    fd = sys_open(current, FILE1 ,O_CREAT | O_RDWR, 0x755);
    assert(fd == 0);

    reset_fs();
}


void test_given_open_when_openned_max_exceeds_should_return_emfile(){
    int ret;
    int i;

    for (i = 0; i < OPEN_MAX; i++){
        ret = sys_open(current, "/", O_RDONLY, 0);
        assert(i >= 0);
    }
    ret = sys_open(current, "/", O_RDONLY, 0);
    assert(ret == -EMFILE);

    reset_fs();
}

void test_given_open_when_openned_exceeds_system_limit_should_return_enfile(){
    int ret;
    struct proc p2, p3;
    int i;
    emulate_fork(current, &p2);
    emulate_fork(current, &p3);
    assert(OPEN_MAX * 2 == NR_FILPS);

    for (i = 0; i < OPEN_MAX; i++){
        ret = sys_open(current, "/", O_RDONLY, 0);
        assert(i >= 0);
    }
    for (i = 0; i < OPEN_MAX; i++){
        ret = sys_open(&p2, "/", O_RDONLY, 0);
        assert(ret >= 0);
    }
    ret = sys_open(&p3, "/", O_RDONLY, 0);
    assert(ret == -ENFILE);

    reset_fs();
}

void test_given_open_when_flag_write_and_path_directory_should_return_eisdir(){
    int ret = sys_open(current, "/", O_WRONLY, 0);
    assert(ret == -EISDIR);
}

void test_given_creat_when_file_not_present_should_return_0(){
    int fd;
    
    fd = sys_creat(current, FILE1 , O_RDWR);
    assert(fd == 0);

    reset_fs();
}

void test_given_creat_when_file_present_should_return_eexist(){
    int fd;
    
    fd = sys_creat(current, FILE1 , O_RDWR);
    assert(fd == 0);

    fd = sys_creat(current, FILE1 , O_RDWR);
    assert(fd == -EEXIST);

    reset_fs();
}

void test_given_close_when_file_closed_should_return_ebadf(){
    int fd, ret;
    
    fd = sys_open(current, FILE1 ,O_CREAT | O_RDWR, 0x755);
    assert(fd == 0);

    _close_delete_file(fd, FILE1);

    ret = sys_close(current, fd);
    assert(ret == -EBADF);

    reset_fs();
}

void test_given_read_when_fd_is_closed_return_ebadf(){
    int ret, fd;
    
    fd = sys_creat(current, FILE1 , O_RDWR);
    assert(fd == 0);

    ret = sys_close(current, fd);
    assert(ret == 0);

    ret = sys_read(current, fd, buffer, PAGE_LEN);
    assert(ret == -EBADF);

    reset_fs();
}

void test_given_write_when_fd_is_closed_return_ebadf(){
    int ret, fd;
    
    fd = sys_creat(current, FILE1 , O_RDWR);
    assert(fd == 0);

    ret = sys_close(current, fd);
    assert(ret == 0);

    ret = sys_write(current, fd, buffer, PAGE_LEN);
    assert(ret == -EBADF);

    reset_fs();
}

void test_given_open_when_deleting_file_should_return_error(){
    int ret, fd;
    
    fd = sys_creat(current, FILE1 , O_RDWR);
    assert(fd == 0);

    _close_delete_file(fd, FILE1);

    ret = sys_open(current, FILE1 , O_RDWR, 0775);
    assert(ret == -ENOENT);

    reset_fs();
}

void test_given_dup_when_dupping_file_should_result_in_same_fd(){
    int ret, fd, fd2;
    
    fd = sys_open(current, FILE1 ,O_CREAT | O_RDWR, 0775);
    assert(fd == 0);
    
    fd2 = sys_dup(current, fd);
    assert(fd2 == fd + 1);

    ret = sys_write(current, fd, "abc", 3);
    assert(ret == 3);

    ret = sys_write(current, fd, "def", 4);
    assert(ret == 4);

    ret = sys_read(current, fd, buffer, 100);
    assert(ret == 0);

    ret = sys_read(current, fd2, buffer, 100);
    assert(ret == 0);

    ret = sys_lseek(current, fd2, 0, SEEK_SET);
    assert(ret == 0);

    ret = sys_read(current, fd, buffer, 100);
    assert(ret == 7);
    assert(strcmp(buffer, "abcdef") == 0);

    ret = sys_lseek(current, fd, 0, SEEK_SET);
    assert(ret == 0);

    ret = sys_read(current, fd2, buffer, 100);
    assert(ret == 7);
    assert(strcmp(buffer, "abcdef") == 0);

    reset_fs();
}

void test_given_read_when_open_and_closing_file_should_persistted_data(){
    int ret, fd;
    memset(buffer, 0xf, PAGE_LEN);

    fd = sys_open(current, FILE1 , O_CREAT | O_RDONLY, 0x0775);
    assert(fd == 0);
    
    ret = sys_write(current, fd, buffer, PAGE_LEN);
    assert(ret == PAGE_LEN);

    ret = sys_write(current, fd, buffer, PAGE_LEN);
    assert(ret == PAGE_LEN);

    ret = sys_close(current, fd);
    assert(ret == 0);

    fd = sys_open(current, FILE1 , O_RDONLY, 0x0775);
    assert(file_size(current, fd) == PAGE_LEN * 2);

    ret = sys_read(current, fd, buffer2, PAGE_LEN);
    assert(ret == PAGE_LEN);
    assert(memcmp(buffer, buffer2, PAGE_LEN) == 0);

    ret = sys_read(current, fd, buffer2, PAGE_LEN);
    assert(ret == PAGE_LEN);
    assert(memcmp(buffer, buffer2, PAGE_LEN) == 0);

    ret = sys_read(current, fd, buffer2, PAGE_LEN);
    assert(ret == 0);

    reset_fs();
}

void test_given_read_when_o_direct_open_and_closing_file_should_persistted_data(){
    int ret, fd;
    memset(buffer, 0xf, PAGE_LEN);

    fd = sys_open(current, FILE1 , O_CREAT | O_RDONLY | O_DIRECT, 0x0775);
    assert(fd == 0);
    
    ret = sys_write(current, fd, buffer, PAGE_LEN);
    assert(ret == PAGE_LEN);

    ret = sys_write(current, fd, buffer, PAGE_LEN);
    assert(ret == PAGE_LEN);

    ret = sys_close(current, fd);
    assert(ret == 0);

    fd = sys_open(current, FILE1 , O_RDONLY | O_DIRECT, 0x0775);
    assert(file_size(current, fd) == PAGE_LEN * 2);

    ret = sys_read(current, fd, buffer2, PAGE_LEN);
    assert(ret == PAGE_LEN);
    assert(memcmp(buffer, buffer2, PAGE_LEN) == 0);

    ret = sys_read(current, fd, buffer2, PAGE_LEN);
    assert(ret == PAGE_LEN);
    assert(memcmp(buffer, buffer2, PAGE_LEN) == 0);

    ret = sys_read(current, fd, buffer2, PAGE_LEN);
    assert(ret == 0);

    reset_fs();
}


void test_given_access_when_file_not_exist_should_return_enoent(){
    int ret = sys_access(current, FILE1, F_OK);
    assert(ret == -ENOENT);

    reset_fs();
}

void test_given_access_when_file_exists_should_return_0(){
    int ret;
    ret = sys_creat(current, FILE1, 0x755);
    assert(ret == 0);

    ret = sys_access(current, FILE1, F_OK);
    assert(ret == 0);

    reset_fs();
}

void test_given_access_when_folder_exists_should_return_0(){
    int ret;
    ret = sys_mkdir(current, DIR_NAME, 0x755);
    assert(ret == 0);

    ret = sys_access(current, DIR_NAME, F_OK);
    assert(ret == 0);

    reset_fs();
}

void test_given_access_when_under_folder_should_return_enoent(){
    int ret;
    ret = sys_mkdir(current, DIR_NAME, 0x755);
    assert(ret == 0);

    ret = sys_access(current, DIR_FILE1, F_OK);
    assert(ret == -ENOENT);

    reset_fs();
}

void test_given_link_stat_when_two_files_are_linked_should_return_same(){
    int fd, ret;
    struct stat statbuf, statbuf2;

    fd = sys_creat(current, FILE1, O_RDWR);
    assert(fd == 0);

    ret = sys_link(current, FILE1, FILE2);
    assert(ret == 0);

    ret = sys_stat(current, FILE1, &statbuf);
    assert(ret == 0);

    ret = sys_stat(current, FILE2, &statbuf2);
    assert(ret == 0);
    assert(statbuf.st_ino == statbuf2.st_ino);
    assert(statbuf.st_dev == statbuf2.st_dev);
    assert(statbuf.st_nlink == 2);

    reset_fs();
}


void test_given_link_stat_when_one_file_deleted_should_return_1_nlink(){
    int fd, ret;
    struct stat statbuf;

    fd = sys_creat(current, FILE1, O_RDWR);
    assert(fd == 0);

    ret = sys_link(current, FILE1, FILE2);
    assert(ret == 0);

    ret = sys_stat(current, FILE1, &statbuf);
    assert(ret == 0);
    assert(statbuf.st_nlink == 2);

    ret = sys_unlink(current, FILE1);
    assert(ret == 0);

    ret = sys_stat(current, FILE2, &statbuf);
    assert(ret == 0);
    assert(statbuf.st_nlink == 1);

    reset_fs();
}

void test_given_chdir_when_dir_not_present_should_return_eexist(){
    int ret = sys_chdir(current, "/not_exist");
    assert(ret == -EEXIST);
}

void test_given_chdir_when_path_is_file_should_return_eexist(){
    int fd = sys_creat(current, FILE1, O_RDWR);
    assert(fd == 0);

    int ret = sys_chdir(current, FILE1);
    assert(ret == -ENOTDIR);

    reset_fs();
}

void test_given_chdir_when_dir_is_valid_should_succeed(){
    struct stat statbuf;
    int ret = sys_mkdir(current, DIR_NAME, O_RDWR);
    assert(ret == 0);

    ret = sys_chdir(current, DIR_NAME);
    assert(ret == 0);

    ret = sys_stat(current, DIR_NAME, &statbuf);
    assert(ret == 0);
    assert(current->fp_workdir->i_num == statbuf.st_ino);

    reset_fs();
}

void test_given_chmod_when_file_not_present_should_return_enonent(){
    int ret = sys_chmod(current, "/notexists", O_RDONLY);
    assert(ret == -ENOENT);
}

void test_given_chmod_stat_when_file_present_should_return_0(){
    int fd, ret;
    struct stat statbuf;

    fd = sys_creat(current, FILE1, O_RDWR);
    assert(fd == 0);

    ret = sys_chmod(current, FILE1, 0x777);
    assert(ret == 0);

    ret = sys_stat(current, FILE1, &statbuf);
    assert(ret == 0);
    assert(statbuf.st_mode == 0x777);

    reset_fs();
}

void test_given_chmod_stat_when_folder_present_should_return_0(){
    int fd, ret;
    struct stat statbuf;

    fd = sys_mkdir(current, DIR_NAME, O_RDWR);
    assert(fd == 0);

    ret = sys_chmod(current, DIR_NAME, 0x777);
    assert(ret == 0);

    ret = sys_stat(current, DIR_NAME, &statbuf);
    assert(ret == 0);
    assert(statbuf.st_mode == 0x777);

    reset_fs();
}

void test_given_getdents_when_no_files_in_folder_should_return_default_files(){
    struct dirent dir[5];
    int i;
    int fd = sys_open(current, "/", O_RDONLY, 0);
    int ret = sys_getdents(current, fd, dir, 5);
    assert(ret == sizeof(struct dirent) * 2);
    for(i = 0; i < 2; i++){
        assert(char32_strcmp(dir[i].d_name, dirent_array[i]) == 0);
    }

    ret = sys_close(current, fd);
    assert(ret == 0);
}

void test_given_getdents_when_files_in_folder_should_return_files(){
    int ret, fd, fd2, i;
    struct dirent dir[5];

    ret = sys_mkdir(current, DIR_NAME, O_RDWR);
    assert(ret == 0);

    fd = sys_creat(current, DIR_FILE1, O_RDWR);
    assert(fd == 0);

    ret = sys_link(current, DIR_FILE1, DIR_FILE2);
    assert(ret == 0);

    fd2 = sys_open(current, DIR_NAME, O_RDONLY, 0);
    assert(fd2 == 1);

    ret = sys_getdents(current, fd2, dir, 5);
    size_t dirent_size = sizeof(struct dirent);
    size_t desired = dirent_size * 4;
    assert(ret == desired);
    for (i = 0; i < 4; ++i) {
        assert(char32_strcmp(dir[i].d_name, dirent_array[i]) == 0);
    }

    ret = sys_getdents(current, fd2, dir, 10);
    assert(ret == 0);

    reset_fs();
}

void test_given_mknod_when_path_valid_should_return_0(){
    int ret = sys_mknod(current, TTY_PATH, O_RDWR, TTY_DEV);
    assert(ret == 0);

    reset_fs();
}

void test_given_dev_open_when_file_is_driver_should_return_from_driver(){
    int ret = sys_mknod(current, TTY_PATH, O_RDWR, TTY_DEV);
    assert(ret == 0);

    TTY_OPEN_CALLED = false;

    int fd = sys_open(current, TTY_PATH, O_EXCL | O_RDWR, 0);
    assert(fd == 0);
    assert(TTY_OPEN_CALLED == true);

    reset_fs();
}

void test_given_dev_read_when_file_is_driver_should_return_from_driver(){
    int ret = sys_mknod(current, TTY_PATH, O_RDWR, TTY_DEV);
    assert(ret == 0);

    int fd = sys_open(current, TTY_PATH, O_EXCL | O_RDWR, 0);
    assert(fd == 0);

    ret = sys_read(current, fd, buffer, 3);
    assert(ret == TTY_RETURN);

    reset_fs();
}

void test_given_dev_write_when_file_is_driver_should_return_from_driver(){
    int ret = sys_mknod(current, TTY_PATH, O_RDWR, TTY_DEV);
    assert(ret == 0);

    int fd = sys_open(current, TTY_PATH, O_EXCL | O_RDWR, 0);
    assert(fd == 0);

    ret = sys_write(current, fd, buffer, 3);
    assert(ret == TTY_RETURN);

    reset_fs();
}

void test_given_dev_close_when_file_is_driver_should_return_from_driver(){
    int ret = sys_mknod(current, TTY_PATH, O_RDWR, TTY_DEV);
    assert(ret == 0);

    int fd = sys_open(current, TTY_PATH, O_EXCL | O_RDWR, 0);
    assert(fd == 0);

    ret = sys_close(current, fd);
    assert(ret == TTY_RETURN);

    reset_fs();
}

void test_given_dev_dup_when_file_is_driver_should_return_from_driver(){
    int ret = sys_mknod(current, TTY_PATH, O_RDWR, TTY_DEV);
    assert(ret == 0);

    int fd = sys_open(current, TTY_PATH, O_EXCL | O_RDWR, 0);
    assert(fd == 0);

    int fd2 = sys_dup2(current, fd, 1);
    assert(1 == fd2);

    ret = sys_write(current, fd, buffer, 3);
    assert(ret == TTY_RETURN);

    ret = sys_write(current, fd2, buffer, 3);
    assert(ret == TTY_RETURN);

    ret = sys_read(current, fd, buffer, 3);
    assert(ret == TTY_RETURN);

    ret = sys_read(current, fd2, buffer, 3);
    assert(ret == TTY_RETURN);

    ret = sys_close(current, fd);
    assert(ret == TTY_RETURN);

    ret = sys_close(current, fd2);
    assert(ret == TTY_RETURN);

    reset_fs();
}

void test_when_zone_full_should_return_enospc(){
    int fd = sys_open(current, FILE1, O_CREAT | O_RDWR, 0x755);
    assert(fd == 0);

    int remaining_bytes = NR_TZONES * BLOCK_SIZE;
    char _buffer[remaining_bytes];
    int ret = sys_write(current, fd, _buffer, remaining_bytes);
    assert(ret == remaining_bytes);

    ret = sys_write(current, fd, _buffer, 1);
    assert(ret == -EFBIG);
}

void test_given_zone_iterator_should_return(){
    struct zone_iterator iter;
    struct device* dev = get_dev(ROOT_DEV);
    struct inode* root = get_inode(ROOT_INODE_NUM, dev);
    int ret = init_zone_iterator(&iter, root, 0);
    assert(ret == 0);
    assert(iter.i_inode == root);
    assert(iter.i_zone_idx == 0);
}