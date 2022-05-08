#include <stddef.h>
#include <sys/debug.h>
#include <stdio.h>

const char** _environ = NULL;
/**
 * @brief pointer to environment variable is stored at the bottom of stack by convention
 * refer to build_user_stack in kernel/system/do_execve.c
 * 
 */

#define __ALIGN1K(x) 	    (((((x))>>10)<<10)+1023)
#define __get_env_address() (__ALIGN1K((unsigned long)get_sp()))
#define __get_env_ptr()     (*((const char ***) (unsigned long)__get_env_address()))

const char** get_environ(){
    if(!_environ){
        _environ = __get_env_ptr();
    }
    return _environ;
}
