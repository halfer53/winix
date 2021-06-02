#include "fs.h"
#include <winix/list.h>


struct list_head devices_list;

void init_dev(){
    INIT_LIST_HEAD(&devices_list);
}

void init_drivers(){
    struct device* dev;
    list_for_each_entry(struct device, dev, &devices_list, list){
        if(dev->dops && dev->dops->dev_init){
            dev->dops->dev_init();
        }
    }
}

int register_device(struct device* dev, const char* name, dev_t id, mode_t type,struct device_operations* dops, struct filp_operations* fops){
    dev->init_name = name;
    dev->dev_id = id;
    dev->device_type = type;
    dev->dops = dops;
    dev->fops = fops;
    list_add(&dev->list,&devices_list);
    return OK;
}

struct device* get_dev(dev_t dev){
    struct device* ret;
    list_for_each_entry(struct device, ret, &devices_list, list){
        if(ret->dev_id == dev){
            return ret;
        }
    }
    return NULL;
}




