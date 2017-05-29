#include <stdio.h>
#include <sys/rex.h>

/**
 * IMPORTANT: printf is a system call in winix, do not cross compile this file unless you know what you are doing
 * 
 */
/**
 * Writes a character to serial port 1.
 **/
 
int putc(const int c) {
	//TODO: buffer output and print via system calls.
	while(!(RexSp1->Stat & 2));
	RexSp1->Tx = c;
	return 0;
}

/**
 * Reads a character from serial port 1.
 **/
int getc() {
	//TODO: user interrupt-driven I/O
	while(!(RexSp1->Stat & 1));
	return RexSp1->Rx;
}

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
 * Writes a string to the serial port.
 **/
static void puts(const char *s) {
	while(*s)
		putc(*s++);
}


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
