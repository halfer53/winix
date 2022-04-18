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

#ifndef _SIZE_T
#define	_SIZE_T
typedef unsigned int	size_t;		/* type returned by sizeof */
#endif /* _SIZE_T */

#ifndef __wchar_t__
#define __wchar_t__
typedef unsigned int wchar_t;
#endif

#ifndef NULL
#define	NULL		((void *)0)
#endif

#define offsetof(st, m) \
    ((size_t)((char *)&((st *)0)->m - (char *)0))

#endif
