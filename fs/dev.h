#ifndef _DEV_DEV_H_
#define _DEV_DEV_H_ 1

#define DEV_READ    1
#define DEV_WRITE   2

#define align_sector(x) (((((x)-1)>>9)<<9))

#include <sys/types.h>
#include <winix/type.h>
#include <fs/type.h>

struct device{
    dev_t dev_id;
    const char* init_name;
    struct list_head list;
    int (*dev_register) ();
    int (*dev_destroy) ();
    struct filp_operations ops;
};

struct file_system{
    struct device* device;
    struct superblock* sb;
    const char* type;
    struct list_head list;
};

extern struct device* devices;

void init_dev();
void init_root_fs();
int dev_io_read(disk_word_t *buf, block_t blocknr);
int dev_io_write(disk_word_t *buf, block_t blocknr);

#endif