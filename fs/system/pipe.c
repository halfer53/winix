//
// Created by bruce on 25/04/20.
//

#include "../fs.h"
#include <winix/list.h>
#include <winix/sigsend.h>
#include <winix/mm.h>

struct device pipe_dev;
static char* name = "pipe";
static ino_t next_inum = 2;
static dev_t pipe_devid = MAKEDEV(2, 1);
#define PIPE_LIMIT  (PAGE_LEN)

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

int do_pipe(struct proc* who, struct message* msg){
    int* fds;
    vptr_t* vp = msg->m1_p1;
    if(!is_vaddr_accessible(vp, who))
        return EFAULT;
    fds = (int*)get_physical_addr(vp, who);
    return sys_pipe(who, fds);
}

int sys_pipe(struct proc* who, int fd[2]){
    int ret1, ret2, ret = 0;
    struct filp *file1, *file2;
    struct inode* inode;
    struct filp_pipe* pipe;
    char* ptr;

    ptr = (char *)get_free_pages(1, GFP_HIGH);
    if(!ptr)
        return ENOMEM;

    pipe = kmalloc(sizeof(struct filp_pipe));
    if(!pipe){
        ret = ENOMEM;
        goto failed_filp_pipe;
    }
    pipe->data = ptr;
    pipe->pos = 0;

    inode = get_free_inode_slot();
    if(!inode){
        ret = EMFILE;
        goto failed_filp_slot;
    }
    inode->flags |= INODE_FLAG_PIPE;
    inode->i_count = 2;
    inode->i_nlinks = 1;
    init_inode_non_disk(inode, get_next_ino(), &pipe_dev, NULL);

    ret1 = set_filp(who, &file1, inode);
    if(ret1 < 0){
        ret = ret1;
        goto failed_filp1;
    }
    file1->pipe_mode = FILP_PIPE_READ;
    file1->pipe = pipe;
    init_filp_by_inode(file1, inode);
    who->fp_filp[ret1] = file1;
    fd[0] = ret1;

    ret2 = set_filp(who, &file2, inode);
    if(ret2 < 0){
        ret = ret2;
        goto failed_filp2;
    }
    file2->pipe_mode = FILP_PIPE_WRITE;
    file2->pipe = pipe;
    init_filp_by_inode(file2, inode);
    who->fp_filp[ret2] = file2;
    fd[1] = ret2;

    INIT_LIST_HEAD(&inode->pipe_writing_list);
    INIT_LIST_HEAD(&inode->pipe_reading_list);
    // KDEBUG(("new pipe ret %d %d with inode %d for proc %d\n", ret1, ret2, inode->i_num, who->proc_nr));
    return OK;

    failed_filp2:
    who->fp_filp[ret1] = NULL;
    release_filp(file1);

    failed_filp1:
    inode->i_count = 0;

    failed_filp_slot:
    kfree(pipe);

    failed_filp_pipe:
    release_pages((ptr_t *)ptr, 1);
    return ret;
}

struct pipe_waiting* get_next_waiting(struct list_head *waiting_list){
    struct pipe_waiting *p1, *p2;
    if(!list_empty(waiting_list)){
        return list_entry(waiting_list->next, struct pipe_waiting, list);
    }
    return NULL;
}

void shift_pipe_data(struct filp* file, off_t i){
    char *data = file->pipe->data;
    off_t j = 0, k, end = file->pipe->pos;
    while(i < end){
        data[j] = data[i];
        data[i] = '\0';
        i++;
        j++;
    }
    k = j;
    while(k < end){
        data[k++] = '\0';
    }
    file->pipe->pos = j;
}

static int _pipe_read(struct proc* who, struct filp *filp, char *data, size_t count, off_t offset){
    int ret;
    off_t i;
    char *pipe_data = filp->pipe->data;
    off_t end = count < filp->pipe->pos ? count : filp->pipe->pos;
    for(i = 0; i < end; i++){
        *data++ = pipe_data[i];
    }
    ret = (int)(i);
    // KDEBUG(("%s[%d] pipe read ret %d \n", 
    //     who->name, curr_syscall_caller->proc_nr, ret));
    shift_pipe_data(filp, i);
    return ret;
}

static int _pipe_write(struct proc* who, struct filp *filp, char *data, size_t count, off_t offset){
    int ret, i;
    char *data_start = filp->pipe->data, *p = data;
    off_t off = filp->pipe->pos, end;
    end = (off + count) < PIPE_LIMIT ? (off + count) : PIPE_LIMIT;
    for(i = off; i < end; i++){
        data_start[i] = *p++;
    }
    ret = (int)(i - off);
    filp->pipe->pos += ret;
    // KDEBUG((" proc %d writing data 0x%x ret %d pipe->pos %d\n",
    //     who->proc_nr, data, ret, filp->pipe->pos));
    return ret;
}

int pipe_read ( struct filp *filp, char *data, size_t count, off_t offset){
    int ret, ret2;
    struct pipe_waiting* next;
    struct message msg;
    struct inode* ino = filp->filp_ino;

    if(filp->pipe_mode == FILP_PIPE_WRITE)
        return 0;

    offset = 0; //Pipe always read from start
    if(filp->pipe->pos == 0){
        if(filp->filp_flags & O_NONBLOCK)
            return 0;
        if(ino->i_count == 1) // write end is closed
            return 0;

        next = (struct pipe_waiting*)kmalloc(sizeof(struct pipe_waiting));
        if(!next)
            return ENOMEM;
        curr_syscall_caller->flags |= STATE_WAITING;
        next->who = curr_syscall_caller;
        next->filp = filp;
        next->data = data;
        next->count = count;
        next->offset = offset;
        next->sys_call_num = READ;
        list_add(&next->list, &ino->pipe_reading_list);
        // KDEBUG(("pipe: proc %d reading inode num %d is blocked \n", curr_syscall_caller->pid, filp->filp_ino->i_num));
        return SUSPEND;
    }
    ret = _pipe_read(curr_syscall_caller, filp, data, count, offset);

    next = get_next_waiting(&ino->pipe_writing_list);
    if(next!= NULL ){
        // KDEBUG(("pipe: proc %d is awaken for writing\n", next->who->proc_nr));
        list_del(&next->list);
        ret2 = _pipe_write(next->who, next->filp, next->data, next->count, next->offset);
        next->who->flags &= ~STATE_WAITING;
        syscall_reply2(next->sys_call_num, ret2, next->who->proc_nr, &msg);
        kfree(next->data);
        kfree(next);
    }
    return ret;
}



int pipe_write ( struct filp *filp, char *data, size_t count, off_t offset){
    int ret, ret2;
    struct pipe_waiting* next;
    struct message msg;
    struct inode* ino = filp->filp_ino;

    if(filp->pipe_mode == FILP_PIPE_READ){
        return 0;
    }

    if(filp->filp_ino->i_count == 1) {
        int signum = SIGPIPE;
        if(curr_syscall_caller->sig_table[signum].sa_handler == SIG_IGN){
            return EPIPE;
        }
        send_sig(curr_syscall_caller, signum);
        return SUSPEND;
    }

    if(count > PIPE_LIMIT)
        return ENOSPC;

    if(count > (PIPE_LIMIT - filp->pipe->pos)){
        char *p, *p2;
        int len = count;
        if(filp->filp_flags & O_NONBLOCK)
            return 0;
        
        next = (struct pipe_waiting*)kmalloc(sizeof(struct pipe_waiting));
        if(!next)
            return ENOMEM;
        p = (char *)kmalloc(count);
        if(!next){
            kfree(next);
            return ENOMEM;
        }
        p2 = p;
        while(len--){
            *p2++ = *data++;
        }
        curr_syscall_caller->flags |= STATE_WAITING;
        next->who = curr_syscall_caller;
        next->filp = filp;
        next->data = p;
        next->count = count;
        next->offset = offset;
        next->sys_call_num = WRITE;
        list_add(&next->list, &ino->pipe_writing_list);
        // KDEBUG(("pipe: proc %d writing from 0x%x %d bytes is blocked\n",
        //         curr_syscall_caller->pid, data, count));
        // KDEBUG(("\n"));
        return SUSPEND;
    }
    ret = _pipe_write(curr_syscall_caller, filp, data, count, offset);
    next = get_next_waiting(&ino->pipe_reading_list);
    if(next!= NULL){
        // KDEBUG(("pipe: proc %d is awaken for reading\n", next->who->proc_nr));
        list_del(&next->list);
        ret2 = _pipe_read(next->who, next->filp, next->data, next->count, next->offset);
        next->who->flags &= ~STATE_WAITING;
        syscall_reply2(next->sys_call_num, ret2, next->who->proc_nr, &msg);
        kfree(next);
    }
    return ret;
}

int pipe_open ( struct device* dev, struct filp *file){
    return 0;
}

int pipe_close ( struct device* dev, struct filp *file){
    int ret;
    struct inode* ino = file->filp_ino;
    struct pipe_waiting* next;
    struct list_head *waiting;
    struct message msg;
    int (*fn)(struct proc*, struct filp*, char *, size_t, off_t);
    file->filp_count -= 1;
    
    if(file->filp_count == 0){
        ino->i_count -= 1;
        if(ino->i_count == 0){
            // KDEBUG(("Releasing pipe %d\n", file->filp_ino->i_num));
            release_pages((ptr_t *)file->pipe->data, 1);
            kfree(file->pipe);
            
            // release inode
            memset(ino, 0, sizeof(struct inode));
        }else{
            if(file->pipe_mode == FILP_PIPE_READ){
                fn = _pipe_write;
                waiting = &ino->pipe_writing_list;
            }else{
                fn = _pipe_read;
                waiting = &ino->pipe_reading_list;
            }
            next = get_next_waiting(waiting);
            if(next!= NULL){
                // KDEBUG(("next waiting %s\n", next->who->name));
                list_del(&next->list);
                ret = fn(next->who, next->filp, next->data, next->count, next->offset);
                next->who->flags &= ~STATE_WAITING;
                syscall_reply2(next->sys_call_num, ret, next->who->proc_nr, &msg);
                kfree(next);
            }
        }
    }

    // KDEBUG(("%s[%d] close file %d, mode %d, count %d ino count %d\n", curr_syscall_caller->name, curr_syscall_caller->proc_nr,
    //     file->filp_ino->i_num, file->pipe_mode, file->filp_count, ino->i_count));
    return 0;
}

int pipe_dev_open(){
    return 0;
}


static struct filp_operations pipe_fops = {pipe_open, pipe_read, pipe_write, pipe_close};

void init_pipe(){
    register_device(&pipe_dev, name, pipe_devid, S_IFIFO, NULL, &pipe_fops);
}
