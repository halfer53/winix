/**
 * 
 * kernel common definitions
 *
 * @author Bruce Tan
 * @email brucetansh@gmail.com
 * 
 * @author Paul Monigatti
 * @email paulmoni@waikato.ac.nz
 * 
 * @create date 2017-08-23 06:21:55
 * 
*/
#ifndef _W_COMM_H_
#define _W_COMM_H_ 1

#include <stdbool.h>

// status to suspend caller, reply later
#define SUSPEND         -998     
// Don't reply back to the sender
#define DONTREPLY       -997

#define EXIT_MAGIC      0x10293847

// Major and minor version numbers for WINIX.
#define MAJOR_VERSION 2
#define MINOR_VERSION 6

#define HZ    (60)

#define DO_CLOCKTICK    (1)

// Asm assembly binaries
#define ASM_ADDUI_SP_SP_1       (0x1ee10001)
#define ASM_SYSCALL             (0x200d0000)


#define ZERO_BITS   1
#define ONE_BITS    2

#define _STR(s) (#s)

#define WRITE_ONCE(n,v) (n = v)

#ifndef container_of
/**
 * container_of - cast a member of a structure out to the containing structure
 * @ptr:        the pointer to the member.
 * @type:       the type of the container struct this is embedded in.
 * @member:     the name of the member within the struct.
 *
 */
 #define container_of(ptr, type, member)\
    ((type *)( (char *)ptr - offsetof(type,member) ))
#endif

extern bool trace_syscall;

#endif
