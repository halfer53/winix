#include <fs/fs.h>

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
const char *get_name(const char *old_name, char string[WINIX_NAME_LEN]){
    int c;
    char *np;
    const char*rnp;

    np = string;            /* 'np' points to current position */
    rnp = old_name;        /* 'rnp' points to unparsed string */
    while ( (c = *rnp) == '/'){
      rnp++;    /* skip leading slashes */
    }

    /* Copy the unparsed path, 'old_name', to the array, 'string'. */
    while ( rnp < &old_name[PATH_MAX]  &&  c != '/'   &&  c != '\0') {
      if (np < &string[WINIX_NAME_LEN]){
        *np++ = c;
      }
      c = *++rnp;        /* advance to next character */
    }

    /* To make /usr/ast/ equivalent to /usr/ast, skip trailing slashes. */
    while (c == '/' && rnp < &old_name[PATH_MAX]){
      c = *++rnp;
    }

    if (np < &string[WINIX_NAME_LEN]) *np = '\0';    /* Terminate string */

    if (rnp >= &old_name[PATH_MAX]) {
    //   err_code = ENAMETOOLONG;
      return((char *) 0);
    }
    return(rnp);
}

// given a directory and a name component, lookup in the directory
// and find the corresponding inode
int advance(inode_t *dirp, char string[WINIX_NAME_LEN]){
    struct winix_dirent* dirstream;
    struct dirent_iterator iter;
    int ret = -EINVAL;

    if(*string == '\0')
        return ret;
    iter_dirent_init(&iter, dirp);
//    kdebug("advancing %s in inode %d\n", string, dirp->i_num);
    while(iter_dirent_has_next(&iter)){
        dirstream = iter_dirent_get_next(&iter);
        if(char32_strcmp(dirstream->dirent.d_name, string) == 0){
            ret = dirstream->dirent.d_ino;
            break;
        }
    }
    iter_dirent_close(&iter);
    return ret;
}

int get_parent_inode_num(inode_t *dirp){
    struct winix_dirent* dirstream;
    struct dirent_iterator iter;
    int ret = -EINVAL;

    iter_dirent_init(&iter, dirp);
//    kdebug("advancing %s in inode %d\n", string, dirp->i_num);
    while(iter_dirent_has_next(&iter)){
        dirstream = iter_dirent_get_next(&iter);
        if(char32_strcmp(dirstream->dirent.d_name, dot2) == 0){
            ret = dirstream->dirent.d_ino;
            break;
        }
    }
    iter_dirent_close(&iter);
    return ret;
}

int get_child_inode_name(inode_t* parent, inode_t* child, char string[WINIX_NAME_LEN]){
    struct winix_dirent* dirstream;
    struct dirent_iterator iter;
    int ret = -EINVAL;
    char32_t *p;
    int len = -EINVAL;

    iter_dirent_init(&iter, parent);
//    kdebug("advancing %s in inode %d\n", string, dirp->i_num);
    while(iter_dirent_has_next(&iter)){
        dirstream = iter_dirent_get_next(&iter);
        if(dirstream->dirent.d_ino == child->i_num){
            p = dirstream->dirent.d_name;
            len = char32_strlen(p);
            char32_strlcpy2(string, p, len + 1);
            ret = len;
            break;
        }
    }
    iter_dirent_close(&iter);
    return ret;
}

 

int __eath_path(struct inode* curr_ino, struct inode** last_dir,
        struct inode** ret_ino, const char *path, char string[WINIX_NAME_LEN]){

    inode_t *rip, *new_rip;
    ino_t inum;
    const char *component_name;
    struct device* dev;

    rip = curr_ino;

    /* If dir has been removed or path is empty, return -ENOENT. */
    if (rip->i_nlinks == 0 || *path == '\0') {
        return -ENOENT;
    }
    dev = rip->i_dev;

    while(1){
        if((component_name = get_name(path,string)) == (char *)0){
            return -ENOENT; // bad parsing
        }

        if(*component_name == '\0') {
            if (rip->i_mode & S_IFDIR){
                *last_dir = rip;
                inum = advance(rip, string);
                if(inum == -EINVAL){
                    return 0;
                }
                new_rip = get_inode(inum, rip->i_dev);
                *ret_ino = new_rip;
                return 0;
            }else{
                return -ENOTDIR;
            }
        }

        if(!(rip->i_mode & S_IFDIR))
            return -ENOTDIR; //if one of the pathname in the path is not directory
//        kdebug("path advance %d %s\n", rip->i_num, string);
        inum = advance(rip,string);
        if(inum == -EINVAL) {
            return 0;
        }

        put_inode(rip, false);
        new_rip = get_inode(inum, dev);
//        kdebug("got new rip %d ret %d\n", inum, new_rip->i_num);
        rip = new_rip;
        path = component_name;
    }
    return -ENOENT;
}

int eat_path(struct proc* who, const char *path, struct inode** last_dir, struct inode** ret_ino, char string[WINIX_NAME_LEN]){
    int ret;
    inode_t *curr_dir;
    if(who->fp_workdir == NULL || who->fp_rootdir == NULL){
        struct device* dev = get_dev(ROOT_DEV);
        curr_dir = get_inode(ROOT_INODE_NUM, dev);
        who->fp_workdir = who->fp_rootdir = curr_dir;
    }
    curr_dir = (*path == '/') ? who->fp_rootdir : who->fp_workdir;
    curr_dir->i_count += 1;
    *ret_ino = NULL;
    *last_dir = NULL;

    ret = __eath_path(curr_dir, last_dir, ret_ino, path, string);
    if (*last_dir == NULL)
        return -ENOENT;
    return ret;
}

int get_inode_by_path(struct proc* who, const char *path, struct inode** inode){
    int ret;
    inode_t *lastdir = NULL;
    char string[WINIX_NAME_LEN];

    ret = eat_path(who, path, &lastdir, inode, string);
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

