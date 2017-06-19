#include "fs.h"

char dot1[2] = ".";	/* used for search_dir to bypass the access */
char dot2[3] = "..";	/* permissions for . and ..		    */




/* Given a pointer to a path name in fs space, 'old_name', copy the next
* component to 'string' and pad with zeros.  A pointer to that part of
* the name as yet unparsed is returned.  Roughly speaking,
* 'get_name' = 'old_name' - 'string'.
*
* This routine follows the standard convention that /usr/ast, /usr//ast,
* //usr///ast and /usr/ast/ are all equivalent.
*/
char *get_name(char *old_name, char string[NAME_MAX]){
    register int c;
    register char *np, *rnp;

    np = string;			/* 'np' points to current position */
    rnp = old_name;		/* 'rnp' points to unparsed string */
    while ( (c = *rnp) == '/'){
      rnp++;	/* skip leading slashes */
    }

    /* Copy the unparsed path, 'old_name', to the array, 'string'. */
    while ( rnp < &old_name[PATH_MAX]  &&  c != '/'   &&  c != '\0') {
      if (np < &string[NAME_MAX]){
        *np++ = c;
      }
      c = *++rnp;		/* advance to next character */
    }

    /* To make /usr/ast/ equivalent to /usr/ast, skip trailing slashes. */
    while (c == '/' && rnp < &old_name[PATH_MAX]){
      c = *++rnp;
    }

    if (np < &string[NAME_MAX]) *np = '\0';	/* Terminate string */

    if (rnp >= &old_name[PATH_MAX]) {
      err_code = ENAMETOOLONG;
      return((char *) 0);
    }
    return(rnp);
}

//given a directory and a name component, lookup in the directory
//and find the corresponding inode
inode_t *advance(inode_t *dirp, char string[NAME_MAX]){
    register char *str;
    int i,inum  = 0;
    buf_t *buffer;
    

    //currently only reads the first block
    if( (buffer = get_block(dirp->i_zone[0])) != NULL){
        for(str = (char *)&buffer->block[0]; str < &buffer->block[BLOCK_SIZE]; str+= DIRSIZ){
          str += 8; //skip inode
          if(strcmp(str,string) == 0){
              inum = hex2str(str-8);
              break;
          }
        }
    }

    if(!inum)
        return NIL_INODE;
    
    return get_inode(inum);
    
}

 

inode_t *last_dir(char *path, char string[DIRSIZ]){
    register inode_t *rip, *new_rip;
    register char *component_name;

    rip = *path == '/' ? current_proc->fp_rootdir : current_proc->fp_workdir;

    /* If dir has been removed or path is empty, return ENOENT. */
    if (rip->i_nlinks == 0 || *path == '\0') {
        err_code = ENOENT;
        return(NIL_INODE);
    }

    rip->i_count++;

    while(1){
        if((component_name = get_name(path,string)) == (char *)0){
            return NIL_INODE; //bad parsing
        }

        if(component_name == '\0'){
            if(rip->i_mode & I_DIRECTORY)
                return rip;
            else{
                return NIL_INODE; //bad parsing
            }
        }
        
        new_rip = advance(rip,string);
        if(new_rip == NIL_INODE)
            return NIL_INODE;

        rip = new_rip;
        path = component_name;
    }
}