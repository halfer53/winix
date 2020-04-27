#ifndef _DEV_DEV_H_
#define _DEV_DEV_H_ 1

#define DEV_READ    1
#define DEV_WRITE   2

#define align_sector(x) (((((x)-1)>>9)<<9))

#include <sys/types.h>
#include <winix/type.h>
#include <fs/type.h>

struct device_operations{
    int (*dev_init) ();
    int (*dev_release) ();
    int (*dev_read) (disk_word_t *buf, off_t off, size_t len);
    int (*dev_write) (disk_word_t *buf, off_t off, size_t len);
};

struct device{
    dev_t dev_id;
    const char* init_name;
    mode_t device_type;
    struct list_head list;
    struct device_operations *dops;
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

