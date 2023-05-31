#include <fs/fs.h>
#include <assert.h>
#include "unit_test.h"

void test_given_zone_iterator_should_return(){
    struct zone_iterator iter;
    struct device* dev = get_dev(ROOT_DEV);
    struct inode* root = get_inode(ROOT_INODE_NUM, dev);
    int ret = iter_zone_init(&iter, root);
    assert(ret == 0);
    assert(iter.i_inode == root);
    assert(iter.i_zone_idx == 0);

    ret = iter_zone_close(&iter);
    assert(ret == 0);
}

void test_given_has_next_zone_when_exceed_max_should_return_false(){
    struct zone_iterator iter;
    struct device* dev = get_dev(ROOT_DEV);
    struct inode* root = get_inode(ROOT_INODE_NUM, dev);
    int ret = _iter_zone_init(&iter, root, MAX_ZONES);
    assert(ret == 0);
    
    bool result = iter_zone_has_next(&iter);
    assert(result == false);

    ret = iter_zone_close(&iter);
    assert(ret == 0);
}

void test_given_has_next_zone_when_no_zone_should_return_false(){
    struct zone_iterator iter;

    struct filp* filp;
    int ret = filp_open(curr_scheduling_proc, &filp, FILE1, O_CREAT | O_RDWR, 0755);

    ret = iter_zone_init(&iter, filp->filp_ino);
    assert(ret == 0);
    
    bool result = iter_zone_has_next(&iter);
    assert(result == false);

    ret = iter_zone_get_next(&iter);
    assert(ret == 0);

    ret = iter_zone_get_next(&iter);
    assert(ret == 0);

    ret = iter_zone_close(&iter);
    assert(ret == 0);
}

void test_given_has_next_zone_when_have_zone_should_return_true(){
    struct zone_iterator iter;
    struct device* dev = get_dev(ROOT_DEV);
    struct inode* root = get_inode(ROOT_INODE_NUM, dev);
    int ret = iter_zone_init(&iter, root);
    assert(ret == 0);
    
    bool result = iter_zone_has_next(&iter);
    assert(result == true);

    ret = iter_zone_close(&iter);
    assert(ret == 0);
}

void test_given_has_next_zone_when_zone_exhausted_should_return_false(){
    struct zone_iterator iter;
    struct device* dev = get_dev(ROOT_DEV);
    struct inode* root = get_inode(ROOT_INODE_NUM, dev);
    int ret = iter_zone_init(&iter, root);
    assert(ret == 0);
    
    bool result = iter_zone_has_next(&iter);
    assert(result == true);

    zone_t zone = iter_zone_get_next(&iter);
    assert(zone > 0);

    result = iter_zone_has_next(&iter);
    assert(result == false);

    ret = iter_zone_alloc(&iter);
    assert(ret > 0);

    result = iter_zone_has_next(&iter);
    assert(result == true);

    zone = iter_zone_get_next(&iter);
    assert(zone == ret);

    result = iter_zone_has_next(&iter);
    assert(result == false);

    ret = iter_zone_close(&iter);
    assert(ret == 0);
}



void test_given_has_next_zone_when_alloc_zone_should_continue(){
    struct zone_iterator iter;
    struct filp* filp;
    int i;
    bool result;
    zone_t zone;

    int ret = filp_open(curr_scheduling_proc, &filp, FILE1, O_CREAT | O_RDWR, 0755);
    assert(ret == 0);

    ret = iter_zone_init(&iter, filp->filp_ino);
    assert(ret == 0);

    struct inode* inode = iter.i_inode;
    struct device *dev = inode->i_dev;
    assert(is_inode_in_use(inode->i_num, dev));
    assert(inode->i_count == 2);

    for(i = 0; i < MAX_ZONES; i++){
        result = iter_zone_has_next(&iter);
        assert(result == false);

        ret = iter_zone_alloc(&iter);
        assert(ret > 0);

        result = iter_zone_has_next(&iter);
        assert(result == true);

        zone = iter_zone_get_next(&iter);
        assert(zone == ret);
    }

    result = iter_zone_has_next(&iter);
    assert(result == false);

    ret = iter_zone_alloc(&iter);
    assert(ret == -EFBIG);

    ret = iter_zone_close(&iter);
    assert(ret == 0);
    assert(inode->i_count == 1);

    ret = filp_close(filp);
    assert(ret == 0);
    assert(inode->i_count == 0);

    struct inode* i1 = get_inode(inode->i_zone[NR_DIRECT_ZONE], dev);
    put_inode(i1, false);
    assert(i1);
    assert(is_inode_in_use(i1->i_num, dev));
    assert(i1->i_flags & INODE_FLAG_ZONE);
    for(i = 0; i < NR_TZONES; i++){
        assert(i1->i_zone[i] > 0);
    }

    struct inode* i2 = get_inode(inode->i_zone[NR_DIRECT_ZONE + 1], dev);
    put_inode(i2, false);
    assert(i2);
    assert(is_inode_in_use(i2->i_num, dev));
    assert(i2->i_flags & INODE_FLAG_ZONE);
    for(i = 0; i < NR_TZONES; i++){
        assert(i2->i_zone[i] > 0);
    }

    release_inode(inode);

    assert(inode->i_count == 0);
    assert(!is_inode_in_use(inode->i_num, dev));
    for(i = 0; i < NR_TZONES; i++){
        assert(inode->i_zone[i] == 0);
    }
    assert(i1->i_count == 0);
    assert(i2->i_count == 0);
    assert(!is_inode_in_use(i1->i_num, dev));
    assert(!is_inode_in_use(i2->i_num, dev));
    for(i = 0; i < NR_TZONES; i++){
        assert(i1->i_zone[i] == 0);
    }
    for(i = 0; i < NR_TZONES; i++){
        assert(i2->i_zone[i] == 0);
    }
    
}

void test_given_iter_dirent_has_next_when_has_data_should_return_true(){
    struct dirent_iterator iter;
    char filename[] = "abc";
    int ret = sys_mkdir(curr_scheduling_proc, DIR_NAME, 0755);
    assert(ret == 0);

    int fd = sys_open(curr_scheduling_proc, DIR_NAME, O_RDONLY, 0);
    assert(fd == 0);

    struct filp* filp = curr_scheduling_proc->fp_filp[fd];
    struct inode* inode = filp->filp_ino;
    ret = iter_dirent_init(&iter, inode);
    assert(ret == 0);

    bool result = iter_dirent_has_next(&iter);
    assert(result == true);

    struct winix_dirent* dir = iter_dirent_get_next(&iter);
    assert(char32_strcmp(dir->dirent.d_name, ".") == 0);

    dir = iter_dirent_get_next(&iter);
    assert(char32_strcmp(dir->dirent.d_name, "..") == 0);

    struct inode* newinode = alloc_inode(inode->i_dev, inode->i_dev);
    assert(newinode);
    ret = add_inode_to_directory(curr_scheduling_proc, inode, newinode, filename);
    assert(ret == 0);

    dir = iter_dirent_get_next(&iter);
    assert(char32_strcmp(dir->dirent.d_name, filename) == 0);
}


void test_given_iter_dirent_has_next_when_dirent_exhausted_should_return_false(){
    struct dirent_iterator iter;
    int i, j;
    struct winix_dirent* dir;
    int ret = sys_mkdir(curr_scheduling_proc, DIR_NAME, 0755);
    assert(ret == 0);

    int fd = sys_open(curr_scheduling_proc, DIR_NAME, O_RDONLY, 0);
    assert(fd == 0);

    struct filp* filp = curr_scheduling_proc->fp_filp[fd];
    struct inode* inode = filp->filp_ino;
    ret = iter_dirent_init(&iter, inode);
    assert(ret == 0);

    int dirent_per_block = BLOCK_SIZE / sizeof(struct winix_dirent);
    for(i = 0; i < MAX_ZONES; i++){
        for(j = 0; j < dirent_per_block; j++){
            assert(iter_dirent_has_next(&iter) == true);
            dir = iter_dirent_get_next(&iter);
            assert(dir != NULL);
        }

        assert(iter_dirent_has_next(&iter) == false);
        if (i < MAX_ZONES - 1){
            ret = iter_dirent_alloc(&iter);
            assert(ret >= 0);
            assert(iter_dirent_has_next(&iter) == true);
        }
    }
    ret = iter_dirent_alloc(&iter);
    assert(ret == -EFBIG);
    assert(iter_dirent_has_next(&iter) == false);

    assert(iter_dirent_close(&iter) == 0);
}
