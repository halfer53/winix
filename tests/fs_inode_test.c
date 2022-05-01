#include <fs/fs.h>
#include <assert.h>
#include "unit_test.h"

void test_given_zone_iterator_should_return(){
    struct zone_iterator iter;
    struct device* dev = get_dev(ROOT_DEV);
    struct inode* root = get_inode(ROOT_INODE_NUM, dev);
    int ret = init_zone_iterator(&iter, curr_scheduling_proc, root, 0);
    assert(ret == 0);
    assert(iter.i_inode == root);
    assert(iter.i_zone_idx == 0);

    ret = iter_close(&iter);
    assert(ret == 0);
}

void test_given_has_next_zone_when_exceed_max_should_return_false(){
    struct zone_iterator iter;
    struct device* dev = get_dev(ROOT_DEV);
    struct inode* root = get_inode(ROOT_INODE_NUM, dev);
    int ret = init_zone_iterator(&iter, curr_scheduling_proc, root, MAX_ZONES);
    assert(ret == 0);
    
    bool result = iter_has_next_zone(&iter);
    assert(result == false);

    ret = iter_close(&iter);
    assert(ret == 0);
}

void test_given_has_next_zone_when_no_zone_should_return_false(){
    struct zone_iterator iter;

    struct filp* filp;
    int ret = filp_open(curr_scheduling_proc, &filp, FILE1, O_CREAT | O_RDWR, 0x755);

    ret = init_zone_iterator(&iter, curr_scheduling_proc, filp->filp_ino, 0);
    assert(ret == 0);
    
    bool result = iter_has_next_zone(&iter);
    assert(result == false);

    ret = iter_close(&iter);
    assert(ret == 0);
}

void test_given_has_next_zone_when_have_zone_should_return_true(){
    struct zone_iterator iter;
    struct device* dev = get_dev(ROOT_DEV);
    struct inode* root = get_inode(ROOT_INODE_NUM, dev);
    int ret = init_zone_iterator(&iter, curr_scheduling_proc, root, 0);
    assert(ret == 0);
    
    bool result = iter_has_next_zone(&iter);
    assert(result == true);

    ret = iter_close(&iter);
    assert(ret == 0);
}

void test_given_has_next_zone_when_zone_exhausted_should_return_false(){
    struct zone_iterator iter;
    struct device* dev = get_dev(ROOT_DEV);
    struct inode* root = get_inode(ROOT_INODE_NUM, dev);
    int ret = init_zone_iterator(&iter, curr_scheduling_proc, root, 0);
    assert(ret == 0);
    
    bool result = iter_has_next_zone(&iter);
    assert(result == true);

    zone_t zone = iter_get_next_zone(&iter);
    assert(zone > 0);

    result = iter_has_next_zone(&iter);
    assert(result == false);

    ret = iter_alloc_zone(&iter);
    assert(ret > 0);

    result = iter_has_next_zone(&iter);
    assert(result == true);

    zone = iter_get_next_zone(&iter);
    assert(zone == ret);

    result = iter_has_next_zone(&iter);
    assert(result == false);

    ret = iter_close(&iter);
    assert(ret == 0);
}



// void test_given_has_next_zone_when_alloc_zone_should_continue(){

//     struct zone_iterator iter;
//     struct filp* filp;
//     int i;
//     bool result;
//     zone_t zone;

//     int ret = filp_open(curr_scheduling_proc, &filp, FILE1, O_CREAT | O_RDWR, 0x755);
//     assert(ret == 0);

//     ret = init_zone_iterator(&iter, curr_scheduling_proc, filp->filp_ino, 0);
//     assert(ret == 0);

//     for(i = 0; i < MAX_ZONES; i++){
//         if(i == 6){
//             printf("a");
//         }
//         result = iter_has_next_zone(&iter);
//         assert(result == false);

//         ret = iter_alloc_zone(&iter);
//         assert(ret > 0);

//         result = iter_has_next_zone(&iter);
//         assert(result == true);

//         zone = iter_get_next_zone(&iter);
//         assert(zone == ret);
//     }

//     ret = iter_close(&iter);
//     assert(ret == 0);
    
// }
