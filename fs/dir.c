#include "fs.h"



struct direct sys_read_dir(inode_t *ino){
    
}


char *get_name(char *old_name, char string[NAME_MAX]){
/* Given a pointer to a path name in fs space, 'old_name', copy the next
 * component to 'string' and pad with zeros.  A pointer to that part of
 * the name as yet unparsed is returned.  Roughly speaking,
 * 'get_name' = 'old_name' - 'string'.
 *
 * This routine follows the standard convention that /usr/ast, /usr//ast,
 * //usr///ast and /usr/ast/ are all equivalent.
 */

  register int c;
  register char *np, *rnp;

  np = string;			/* 'np' points to current position */
  rnp = old_name;		/* 'rnp' points to unparsed string */
  while ( (c = *rnp) == '/') rnp++;	/* skip leading slashes */

  /* Copy the unparsed path, 'old_name', to the array, 'string'. */
  while ( rnp < &old_name[PATH_MAX]  &&  c != '/'   &&  c != '\0') {
	if (np < &string[NAME_MAX]) *np++ = c;
	c = *++rnp;		/* advance to next character */
  }

  /* To make /usr/ast/ equivalent to /usr/ast, skip trailing slashes. */
  while (c == '/' && rnp < &old_name[PATH_MAX]) c = *++rnp;

  if (np < &string[NAME_MAX]) *np = '\0';	/* Terminate string */

  if (rnp >= &old_name[PATH_MAX]) {
	err_code = ENAMETOOLONG;
	return((char *) 0);
  }
  return(rnp);
}
 

buf_t *last_dir(char *path, int flag){
    register inode_t *rip;


    rip = *path == '/' ? current_proc->fp_rootdir : current_proc->fp_workdir;

    /* If dir has been removed or path is empty, return ENOENT. */
    if (rip->i_nlinks == 0 || *path == '\0') {
        err_code = ENOENT;
        return(NIL_INODE);
    }

    rip->i_count++;


}