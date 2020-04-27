//
// Created by bruce on 25/04/20.
//

#include "../fs.h"
#include <winix/list.h>


struct device pipe_dev;
static struct filp_operations fops;
static struct device_operations dops;
static char* name = "pipe";
static ino_t next_inum = 2;
#define PIPE_LIMIT  (BLOCK_SIZE)

struct pipe_waiting{
    struct proc* who;
    int sys_call_num;
    struct filp *filp;
    char *data;
    size_t count;
    off_t offset;
    struct list_head list;
};

ino_t get_next_ino(){
    return next_inum++;
}

int set_filp(struct proc* who, struct filp** _file, struct inode* inode){
    int ret, open_slot;
    struct filp* file;
    ret = get_fd(who, 0, &open_slot, _file);
    if(ret)
        return ret;
    return open_slot;
}

int sys_pipe(struct proc* who, int fd[2]){
    int ret1, ret2, ret = 0;
    struct filp *file1, *file2;
    struct inode* inode;
    ptr_t* ptr;

    ptr = kmalloc(PIPE_LIMIT);
    if(!ptr)
        return ENOMEM;

    inode = get_free_inode_slot();
    if(!inode){
        ret = EMFILE;
        goto failed_inode;
    }

    ret1 = set_filp(who, &file1, inode);
    if(ret1 < 0){
        ret = ret1;
        goto failed_filp1;
    }

    who->fp_filp[ret1] = file1;
    fd[0] = ret1;

    ret2 = set_filp(who, &file2, inode);
    if(ret2 < 0){
        ret = ret2;
        goto failed_filp2;
    }
    who->fp_filp[ret2] = file2;
    fd[1] = ret2;

    inode->pip_data = ptr;
    inode->flags |= INODE_FLAG_PIPE;
    inode->i_nlinks = 1;
    init_inode_non_disk(inode, get_next_ino(), &pipe_dev, NULL);
    init_filp_by_inode(file1, inode);
    init_filp_by_inode(file2, inode);
    INIT_LIST_HEAD(&inode->pipe_writing_list);
    INIT_LIST_HEAD(&inode->pipe_reading_list);
    return OK;

    failed_filp2:
    who->fp_filp[ret1] = NULL;
    release_filp(file1);

    failed_filp1:
    inode->i_count = 0;

    failed_inode:
    kfree(ptr);
    return ret;
}

struct pipe_waiting* get_next_waiting(struct list_head *waiting_list){
    struct pipe_waiting *p1, *p2;
    list_for_each_entry_safe(struct pipe_waiting, p1, p2, waiting_list, list){
        list_del(&p1->list);
        return p1;
    }
    return NULL;
}

int __pipe_read(struct proc* who, struct filp *filp, char *data, size_t count, off_t offset){
    int ret, i;
    ptr_t *data_start = filp->filp_ino->pip_data;
    for(i = offset; i < offset + count && i < filp->filp_pos; i++){
        *data++ = data_start[i];
    }
    ret = i - offset;
    filp->filp_pos -= ret;
    return ret;
}

int __pipe_write(struct proc* who, struct filp *filp, char *data, size_t count, off_t offset){
    int ret, i;
    ptr_t *data_start = filp->filp_ino->pip_data;
    off_t off = filp->filp_pos;
    for(i = off; i < off + count && i < PIPE_LIMIT; i++){
        data_start[i] = *data++;
    }
    ret = i - off;
    filp->filp_pos += ret;
    return ret;
}

int pipe_read ( struct filp *filp, char *data, size_t count, off_t offset){
    int ret, ret2;
    struct pipe_waiting* next;
    struct message msg;
    struct inode* ino = filp->filp_ino;

    if(filp->filp_pos == 0 || offset > filp->filp_pos){
        next = (struct pipe_waiting*)kmalloc(sizeof(struct pipe_waiting));
        if(!next)
            return ENOMEM;
        curr_user_proc_in_syscall->flags |= STATE_WAITING;
        next->who = curr_user_proc_in_syscall;
        next->filp = filp;
        next->data = data;
        next->count = count;
        next->offset = offset;
        next->sys_call_num = READ;
        list_add(&next->list, &ino->pipe_reading_list);
        KDEBUG(("proc %d reading inode num %d is blocked \n", curr_user_proc_in_syscall->pid, filp->filp_ino->i_num));
        return SUSPEND;
    }
    ret = __pipe_read(curr_user_proc_in_syscall, filp, data, count, offset);
    if(ret == 0)
        return ret;
    next = get_next_waiting(&ino->pipe_writing_list);
    if(next!= NULL){
        KDEBUG(("Proc %d is awaken for writing\n", next->who->proc_nr));
        ret2 = __pipe_write(next->who, next->filp, next->data, next->count, next->offset);
        next->who->flags &= ~STATE_WAITING;
        syscall_reply2(next->sys_call_num, ret2, next->who->proc_nr, &msg);
        kfree(next);
    }
    return ret;
}



int pipe_write ( struct filp *filp, char *data, size_t count, off_t offset){
    int ret, ret2;
    struct pipe_waiting* next;
    struct message msg;
    struct inode* ino = filp->filp_ino;

    if(count > PIPE_LIMIT)
        return ENOSPC;

    if(count > (PIPE_LIMIT - filp->filp_pos)){
        next = (struct pipe_waiting*)kmalloc(sizeof(struct pipe_waiting));
        if(!next)
            return ENOMEM;

        curr_user_proc_in_syscall->flags |= STATE_WAITING;
        next->who = curr_user_proc_in_syscall;
        next->filp = filp;
        next->data = data;
        next->count = count;
        next->offset = offset;
        next->sys_call_num = WRITE;
        list_add(&next->list, &ino->pipe_reading_list);
        KDEBUG(("proc %d writing inode num %d is blocked \n", curr_user_proc_in_syscall->pid, filp->filp_ino->i_num));
        return SUSPEND;
    }
    ret = __pipe_write(curr_user_proc_in_syscall, filp, data, count, offset);
    if(ret == 0)
        return ret;
    KDEBUG(("proc %d writing %s\n", curr_user_proc_in_syscall->proc_nr, data));
    next = get_next_waiting(&ino->pipe_reading_list);
    if(next!= NULL){
        KDEBUG(("Proc %d is awaken for reading\n", next->who->proc_nr));
        ret2 = __pipe_read(next->who, next->filp, next->data, next->count, next->offset);
        next->who->flags &= ~STATE_WAITING;
        syscall_reply2(next->sys_call_num, ret2, next->who->proc_nr, &msg);
        kfree(next);
    }
    return ret;
}

int pipe_open ( struct inode* ino, struct filp *file){
    return 0;
}

int pipe_close ( struct inode* ino, struct filp *file){
    return 0;
}


int pipe_dev_io_read(disk_word_t *buf, off_t off, size_t len){
    return 0;
}

int pipe_dev_io_write(disk_word_t *buf, off_t off, size_t len){
    return 0;
}

int pipe_dev_init(){
    return 0;
}

int pip_dev_release(){
    return 0;
}


void init_pipe(){
    dops.dev_init = pipe_dev_init;
    dops.dev_read = pipe_dev_io_read;
    dops.dev_write = pipe_dev_io_write;
    dops.dev_release = pip_dev_release;
    fops.open = pipe_open;
    fops.read = pipe_read;
    fops.write = pipe_write;
    fops.close = pipe_close;
    pipe_dev.dops = &dops;
    pipe_dev.fops = &fops;
    register_device(&pipe_dev, name, MAKEDEV(2, 1), S_IFIFO);
}
