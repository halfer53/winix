#include "fs.h"

char dot1[2] = ".";    /* used for search_dir to bypass the access */
char dot2[3] = "..";    /* permissions for . and ..            */




/* Given a pointer to a path name in fs space, 'old_name', copy the next
* component to 'string' and pad with zeros.  A pointer to that part of
* the name as yet unparsed is returned.  Roughly speaking,
* 'get_name' = 'old_name' - 'string'.
*
* This routine follows the standard convention that /usr/ast, /usr// ast,
* // usr/// ast and /usr/ast/ are all equivalent.
*/
char *get_name(char *old_name, char string[NAME_MAX]){
    int c;
    char *np, *rnp;

    np = string;            /* 'np' points to current position */
    rnp = old_name;        /* 'rnp' points to unparsed string */
    while ( (c = *rnp) == '/'){
      rnp++;    /* skip leading slashes */
    }

    /* Copy the unparsed path, 'old_name', to the array, 'string'. */
    while ( rnp < &old_name[PATH_MAX]  &&  c != '/'   &&  c != '\0') {
      if (np < &string[NAME_MAX]){
        *np++ = c;
      }
      c = *++rnp;        /* advance to next character */
    }

    /* To make /usr/ast/ equivalent to /usr/ast, skip trailing slashes. */
    while (c == '/' && rnp < &old_name[PATH_MAX]){
      c = *++rnp;
    }

    if (np < &string[NAME_MAX]) *np = '\0';    /* Terminate string */

    if (rnp >= &old_name[PATH_MAX]) {
    //   err_code = ENAMETOOLONG;
      return((char *) 0);
    }
    return(rnp);
}

// given a directory and a name component, lookup in the directory
// and find the corresponding inode
ino_t advance(inode_t *dirp, char string[NAME_MAX]){
    int i,inum  = 0;
    block_buffer_t *buffer;
    struct dirent* dirstream;

    // currently only reads the first block
    for(i = 0; i < NR_TZONES; i++){
        if(dirp->i_zone[i] > 0 && (buffer = get_block_buffer(dirp->i_zone[i], dirp->i_dev)) != NULL){
            dirstream = (struct dirent*)buffer->block;
            for(; dirstream < (struct dirent* )&buffer->block[BLOCK_SIZE]; dirstream++ ){
                if(strcmp(dirstream->d_name, string) == 0){
                    inum = dirstream->d_ino;
                    put_block_buffer(buffer);
                    return inum;
                }
            }
            put_block_buffer(buffer);
        }
    }
    return ERR;
}

 

inode_t *last_dir(char *path, char string[DIRSIZ]){
    inode_t *rip, *new_rip;
    ino_t inum;
    char *component_name;
    struct device* dev;

    rip = *path == '/' ? current_proc->fp_rootdir : current_proc->fp_workdir;

    /* If dir has been removed or path is empty, return ENOENT. */
    if (rip->i_nlinks == 0 || *path == '\0') {
        // err_code = ENOENT;
        return(NIL_INODE);
    }

    rip->i_count++;
    dev = rip->i_dev;

    while(1){
        if((component_name = get_name(path,string)) == (char *)0){
            return NIL_INODE; // bad parsing
        }

        if(*component_name == '\0') {
            if (rip->i_mode & S_IFDIR){
                return rip;
            }else{
                return NIL_INODE; // bad parsing
            }
        }

        inum = advance(rip,string);
        put_inode(rip, false);
        if(inum == ERR) {
            return NIL_INODE;
        }

        new_rip = get_inode(inum, dev);
        rip = new_rip;
        path = component_name;
    }
    return NULL;
}

inode_t* eat_path(char *path, inode_t** lastd, char string[DIRSIZ]){
    inode_t *inode, *lastdir;
    int inum, lnum;
    struct dirent* curr, *end;
    int i, j;
    block_t bnr;
    struct block_buffer* buf;
    struct superblock* sb;

    lastdir = last_dir(path, string);
    if(lastdir == NULL)
        return NULL;
    *lastd = lastdir;

    for(i = 0; i < NR_TZONES; i++){
        bnr = lastdir->i_zone[i];
        if(bnr == 0){
            continue;
        }
        buf = get_block_buffer(bnr, lastdir->i_dev);
        sb = get_sb(lastdir->i_dev);
        end = (struct dirent*)&buf->block[BLOCK_SIZE];
        curr = (struct dirent*)buf->block;
        while (curr < end){
            if(strcmp(curr->d_name, string) == 0){
                inode = get_inode(curr->d_ino, lastdir->i_dev);
                inum = inode == NULL ? 0 : inode->i_num;
                //XDEBUG(("eath path %s return %d\n", path, inum));
                put_block_buffer(buf);
                return inode;
            }
            curr++;
        }
        put_block_buffer(buf);
    }


    return NULL;
}

bool is_fd_opened_and_valid(struct proc* who, int fd){
    struct filp* file = who->fp_filp[fd];
    if(fd < 0 || fd >= OPEN_MAX){
        return EINVAL;
    }
    if(file == NULL){
        return EINVAL;
    }
}