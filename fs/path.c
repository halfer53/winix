#include "fs.h"

char* dot1 = ".";    /* used for search_dir to bypass the access */
char* dot2 = "..";    /* permissions for . and ..            */




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
int advance(inode_t *dirp, char string[NAME_MAX]){
    int i,inum  = 0;
    block_buffer_t *buffer;
    struct dirent* dirstream;

    if(*string == '\0')
        return ERR;

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

 

int __eath_path(struct inode* curr_ino, struct inode** last_dir,
        struct inode** ret_ino, char *path, char string[DIRSIZ]){

    inode_t *rip, *new_rip;
    ino_t inum;
    char *component_name;
    struct device* dev;

    rip = curr_ino;

    /* If dir has been removed or path is empty, return ENOENT. */
    if (rip->i_nlinks == 0 || *path == '\0') {
        return ENOENT;
    }
    dev = rip->i_dev;

    while(1){
        if((component_name = get_name(path,string)) == (char *)0){
            return ENOENT; // bad parsing
        }

        if(*component_name == '\0') {
            if (rip->i_mode & S_IFDIR){
                *last_dir = rip;
                inum = advance(rip, string);
                if(inum == ERR){
                    return OK;
                }
                new_rip = get_inode(inum, rip->i_dev);
                *ret_ino = new_rip;
                return OK;
            }else{
                return ENOTDIR;
            }
        }

        if(!(rip->i_mode & S_IFDIR))
            return ENOTDIR; //if one of the pathname in the path is not directory

        inum = advance(rip,string);
        if(inum == ERR) {
            return OK;
        }

        put_inode(rip, false);
        new_rip = get_inode(inum, dev);
        rip = new_rip;
        path = component_name;
    }
    return ENOENT;
}

int eat_path(struct proc* who, char *path, struct inode** last_dir, struct inode** ret_ino, char string[DIRSIZ]){
    int ret;
    inode_t *curr_dir;
    curr_dir = (*path == '/') ? who->fp_rootdir : who->fp_workdir;
    curr_dir->i_count += 1;
    *ret_ino = NULL;
    *last_dir = NULL;

    ret = __eath_path(curr_dir, last_dir, ret_ino, path, string);

    return ret;
}

int get_inode_by_path(struct proc* who, char *path, struct inode** inode){
    int ret;
    inode_t *lastdir = NULL;
    char string[DIRSIZ];

    ret = eat_path(who, path, &lastdir, &inode, string);
    put_inode(lastdir, false);
    return ret;
}

bool is_fd_opened_and_valid(struct proc* who, int fd){
    struct filp* file;
    if(fd < 0 || fd >= OPEN_MAX){
        return false;
    }
    file = who->fp_filp[fd];
    if(file == NULL){
        return false;
    }
    return true;
}