/**
 * 
 * A rather limited version of <stddef.h>
 *
 * @author Bruce Tan
 * @email brucetansh@gmail.com
 * 
 * @author Paul Monigatti
 * @email paulmoni@waikato.ac.nz
 * 
 * @create date 2016-09-19
 * 
*/

#ifndef _STDDEF_H_
#define _STDDEF_H_ 1

#ifndef NULL
#define	NULL		((void *)0)
#endif

#ifndef EOF
#define	EOF		(-1)
#endif

#define offsetof(st, m) ((size_t)&(((st *)0)->m))

#endif
