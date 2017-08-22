/**
 * A rather limited version of <stdio.h>
 *
 * Revision History:
 *  2016-09-19		Paul Monigatti			Original
 *  2016-11-20		Bruce Tan			Modified
 **/

#ifndef _STDIO_H_
#define _STDIO_H_ 1

#define	BUFSIZ		1024
#define	EOF		(-1)

int putcharhar(const char c);
int getchar();

#endif
