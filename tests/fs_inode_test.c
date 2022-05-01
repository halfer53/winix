#include <fs/fs.h>
#include <assert.h>

void test_given_zone_iterator_should_return(){
    struct zone_iterator iter;
    struct device* dev = get_dev(ROOT_DEV);
    struct inode* root = get_inode(ROOT_INODE_NUM, dev);
    int ret = init_zone_iterator(&iter, root, 0);
    assert(ret == 0);
    assert(iter.i_inode == root);
    assert(iter.i_zone_idx == 0);
}

void test_given_has_next_zone_when_have_zone_should_return_true(){
    struct zone_iterator iter;
    struct device* dev = get_dev(ROOT_DEV);
    struct inode* root = get_inode(ROOT_INODE_NUM, dev);
    int ret = init_zone_iterator(&iter, root, 0);
    assert(ret == 0);
    
    bool result = has_next_zone(&iter);
    assert(result == true);
}
