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

//status to suspend caller, reply later
#define SUSPEND         -998     
//Don't reply back to the sender
#define DONTREPLY       -997

#define INT_MAX         0x7FFFFFFF
#define INT_MIN         0x80000000
#define UINT_MAX        0xffffffff

#define LONG_MIN (-2147483647L-1)/* minimum value of a long */
#define LONG_MAX  2147483647L	/* maximum value of a long */
#define ULONG_MAX 0xFFFFFFFFL	/* maximum value of an unsigned long */

#define EXIT_MAGIC      0x10293847

//Major and minor version numbers for WINIX.
#define MAJOR_VERSION 2
#define MINOR_VERSION 0

//Predefined, wramp board has 1024 pages in total,
//which equals to 4 * 32
#define MEM_MAP_LEN    4

//Number of words per page
#define PAGE_LEN    1024

#define get_hz()    (60)


#define DO_CLOCKTICK    (1)

#endif
