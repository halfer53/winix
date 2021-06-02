#ifndef _DEV_DEV_H_
#define _DEV_DEV_H_ 1

#define DEV_READ    1
#define DEV_WRITE   2

#define align_sector(x) (((((x)-1)>>9)<<9))

#include <sys/types.h>
#include <winix/type.h>
#include <fs/type.h>
#include <fs/filp.h>
#include <fs/cache.h>
#include <winix/rex.h>

struct device_operations{
    int (*dev_init) ();
    int (*dev_read) (char *buf, off_t off, size_t len);
    int (*dev_write) (char *buf, off_t off, size_t len);
    int (*dev_release) ();
};

struct device{
    dev_t dev_id;
    const char* init_name;
    mode_t device_type;
    struct list_head list;
    struct device_operations *dops;
    struct filp_operations *fops;
    struct block_operations *bops;
    void* private;
};



struct file_system{
    struct device* device;
    struct superblock* sb;
    const char* type;
    struct list_head list;
};

extern struct device devices;
extern struct device *tty_dev;
extern struct device *tty2_dev;
extern struct filp *tty2_filp;
extern struct filp *tty1_filp;

void init_dev();
void init_root_fs();
void init_drivers();
int tty_write_rex(RexSp_t* rex, char* data, size_t len);

#endif

