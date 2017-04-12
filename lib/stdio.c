#include <stdio.h>
#include <sys/ipc.h>
#include <sys/syscall.h>
#include <sys/rex.h>

/**
 * Writes a character to serial port 1.
 **/
// int putc(const int c) {
// 	//TODO: buffer output and print via system calls.
// 	while(!(RexSp1->Stat & 2));
// 	RexSp1->Tx = c;
// 	return 0;
// }

/**
 * Writes a character to serial port 2.
//  **/
int putc2(const int c) {
	//TODO: buffer output and print via system calls.
	while(!(RexSp2->Stat & 2));
	RexSp2->Tx = c;
	return 0;
}

/**
 * Reads a character from serial port 1.
 **/
// int getc() {
// 	//TODO: user interrupt-driven I/O
// 	while(!(RexSp1->Stat & 1));
// 	return RexSp1->Rx;
// }

/**
 * Get a character from serial port 2
 **/
// int getc2() {
// 	int response = 0;
// 	message_t m;
//
// 	m.type = SYSCALL_GETC2;
// 	printf("Sending Message...\r\n");
// 	response = winix_sendrec(SYSTEM_TASK, &m); //TODO: error checking
// 	printf("Response: %d\r\n", response);
// 	return m.i1;
// }

/**
 * Writes a number to the serial port.
 **/
static void putd(int n) {
	int place = 1000000000;

	//zero?
	if(n == 0) {
		putc('0');
		return;
	}

	//negative?
	if(n < 0) {
		putc('-');
		n *= -1;
	}

	//find first digit of number
	while(place > n) {
		place /= 10;
	}

	//print the rest
	while(place) {
		int d = n / place;
		putc(d % 10 + '0');
		place /= 10;
	}
}

/**
 * Writes a number to the serial port.
 **/
// static void putd2(int n) {
// 	int place = 1000000000;
//
// 	//zero?
// 	if(n == 0) {
// 		putc2('0');
// 		return;
// 	}
//
// 	//negative?
// 	if(n < 0) {
// 		putc2('-');
// 		n *= -1;
// 	}
//
// 	//find first digit of number
// 	while(place > n) {
// 		place /= 10;
// 	}
//
// 	//print the rest
// 	while(place) {
// 		int d = n / place;
// 		putc2(d % 10 + '0');
// 		place /= 10;
// 	}
// }

/**
 * Writes a number to the serial port in hex
 **/
static void putx(int n) {
	int i;

	for(i = 28; i >= 0; i -= 4) {
		int d = (n >> i) & 0xf;
		if(d < 10) {
			putc(d + '0');
		}
		else {
			putc(d - 10 + 'A');
		}
	}
}

/**
 * Writes a number to the serial port in hex in serial port 2
 **/
// static void putx2(int n) {
// 	int i;
//
// 	for(i = 28; i >= 0; i -= 4) {
// 		int d = (n >> i) & 0xf;
// 		if(d < 10) {
// 			putc2(d + '0');
// 		}
// 		else {
// 			putc2(d - 10 + 'A');
// 		}
// 	}
// }

/**
 * Writes a string to the serial port.
 **/
static void puts(const char *s) {
	while(*s)
		putc(*s++);
}

/**
 * Writes a string to the serial port 2
 **/
// static void puts2(const char *s) {
// 	while(*s)
// 		putc2(*s++);
// }

/**
 * Budget version of printf
 **/
int printf(const char *format, ...) {
	void *arg = &format;
	arg = ((char*)arg) + 1;

	//TODO: proper formats
	while(*format) {
		if(*format == '%') {
			format++;

			switch(*format) {
				case 'd':
					putd(*((int*)arg));
					arg = ((int*)arg) + 1;
					format++;
					break;

				case 'x':
					putx(*((int*)arg));
					arg = ((int*)arg) + 1;
					format++;
					break;

				case 's':
					puts(*(char **)arg);
					arg = ((char *)arg) + 1;
					format++;
					break;

				default:
					putc(*format++);
			}
		}
		else {
			putc(*format++);
		}
	}
	return 0;
}


// int printf2(const char *format, ...) {
// 	void *arg = &format;
// 	arg = ((char*)arg) + 1;
//
// 	//TODO: proper formats
// 	while(*format) {
// 		if(*format == '%') {
// 			format++;
//
// 			switch(*format) {
// 				case 'd':
// 					putd2(*((int*)arg));
// 					arg = ((int*)arg) + 1;
// 					format++;
// 					break;
//
// 				case 'x':
// 					putx2(*((int*)arg));
// 					arg = ((int*)arg) + 1;
// 					format++;
// 					break;
//
// 				case 's':
// 					puts2(*(char **)arg);
// 					arg = ((char *)arg) + 1;
// 					format++;
// 					break;
//
// 				default:
// 					putc2(*format++);
// 			}
// 		}
// 		else {
// 			putc2(*format++);
// 		}
// 	}
// 	return 0;
// }
