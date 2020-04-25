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
    int (*dev_read) (disk_word_t *buf, off_t off, size_t len);
    int (*dev_write) (disk_word_t *buf, off_t off, size_t len);
    struct filp_operations *fops;
};

struct file_system{
    struct device* device;
    struct superblock* sb;
    const char* type;
    struct list_head list;
};

extern struct device devices;

void init_dev();
void init_root_fs();

#endif