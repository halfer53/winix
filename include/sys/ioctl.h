#ifndef _IOCTL_H_
#define _IOCTL_H_ 1


#define TCGETS		1 /* tcgetattr */
#define TCSETS		2 /* tcsetattr, TCSANOW */
#define TCSETSW		3 /* tcsetattr, TCSADRAIN */
#define TCSETSF		4 /* tcsetattr, TCSAFLUSH */
#define TCSBRK		5	      /* tcsendbreak */
#define TCDRAIN		6		      /* tcdrain */
#define TCFLOW		7	      /* tcflow */
#define TCFLSH		8	      /* tcflush */
#define	TIOCGWINSZ	9
#define	TIOCSWINSZ	10
#define	TIOCGPGRP	11  //tcgetpgrp
#define	TIOCSPGRP	12  //tcsetpgrp
#define TIOCSFON_OLD	13
#define TIOCSFON	14

/* Keyboard ioctls. */
#define KIOCBELL        15
#define KIOCSLEDS       16
#define KIOCSMAP	17

#define TIOCSCTTY   18
#define TIOCNOTTY   19

#define TIOCDISABLEECHO 20
#define TIOCENABLEECHO  21

#endif
