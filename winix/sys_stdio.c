#include <winix/rex.h>
#include <stdbool.h>

/**
 * Writes a character to serial port 1.
 **/
void kputc(const int c) {
	while(!(RexSp1->Stat & 2));
	RexSp1->Tx = c;
}


PRIVATE void kputc2(const int c) {
	while(!(RexSp2->Stat & 2));
	RexSp2->Tx = c;
}

/**
 * Reads a character from serial port 1.
 **/
PRIVATE int kgetc() {
	//TODO: user interrupt-driven I/O
	//Use thread
	while(!(RexSp1->Stat & 1));
	return RexSp1->Rx;
}

PRIVATE void kputx_buf(int n,char *buf) {
	int i;
	int v = 0;

	if(n == 0){
		*buf++ = '0';
		*buf = '\0';
		return;
	}
	for(i = 28; i >= 0; i -= 4) {
		int d = (n >> i) & 0xf;
		if(d)
			v = 1;
		if(d < 10) {
			if(v)
				*buf++ = d + '0';
		}
		else {
			// kputc(d - 10 + 'A');
			*buf++ = d - 10 + 'A';
		}
	}
	*buf = '\0';
}

PRIVATE void kputd_buf(int n, char *buf) {
	int place = 1000000000;

	//zero?
	if(n == 0) {
		// kputc('0');
		*buf++ = '0';
		*buf = '\0';
		return;
	}

	//negative?
	if(n < 0) {
		// kputc('-');
		*buf++ = '-';
		n *= -1;
	}

	//find first digit of number
	while(place > n) {
		place /= 10;
	}

	//print the rest
	while(place) {
		int d = n / place;
		// kputc(d % 10 + '0');
		*buf++ = d % 10 + '0';
		place /= 10;
	}
	*buf = '\0';
}

PRIVATE void kputs_vm_buf(char *s, void *who_rbase, char *buf) {
	char *sp = s;
	sp += (int)who_rbase;
	while(*sp)
		*buf++ = *sp++;
	*buf = '\0';
		// kputc(*sp++);
}

PRIVATE void kputs(const char *s) {
	while(*s)
		kputc(*s++);
}

#define SPACE	' '
#define ZERO	'0'

#define PUT_PADDING(_padding,_token)\
	while(_padding--){	\
		kputc(_token);		\
	}					\


PRIVATE void kprintf_vm(const char *format, void *arg, void *who_rbase){
	char c = *format;
	static char buffer[64];
	char *buf = buffer;
	int padding = 0;
	bool right_padding;

	//TODO: proper formats
	while(*format) {
		if(*format == '%') {
			char prev;
			char token = SPACE;
			format++;
			
			if(*format == '-'){
				format++;
				right_padding = true;
			}else{
				right_padding = false;
			}
			
			if(*format == '*'){
				padding = atoi(*(int *)arg);
				arg = ((int*)arg) + 1;
			}else if(*format >= '0' && *format <= '9'){
				buf = buffer;
				*buf++ = *format++;
				*buf++ = *format++;
				*buf = '\0';
				padding = atoi(buffer);
				buf = buffer;
			}
			prev = *format;
			switch(*format) {
				case 'd':
					kputd_buf(*((int*)arg),buf);
					arg = ((int*)arg) + 1;
					format++;
					break;

				case 'x':
					kputx_buf(*((int*)arg),buf);
					arg = ((int*)arg) + 1;
					format++;
					right_padding = false;
					break;

				case 's':
					kputs_vm_buf(*(char **)arg,who_rbase,buf);
					arg = ((char *)arg) + 1;
					format++;
					break;

				case 'c':
					// kputc(*(int *)arg);
					*buf++ = *(int *)arg;
					*buf = '\0';
					arg = ((char *)arg) + 1;
					format++;
					break;

				default:
					kputc(*format++);
			}
			padding -= strlen(buf);
			if(!right_padding && padding > 0){ //left padding
				if(prev == 'x')
					token = ZERO;
				PUT_PADDING(padding,token);
			}
			kputs(buf);
			if(right_padding && padding > 0){
				PUT_PADDING(padding,token);
			}
			padding = 0;	
		}
		else {
			kputc(*format++);
		}
	}
}

void kprintf(const char *format, ...) {
	void *arg = &format;
	arg = ((char*)arg) + 1;

	kprintf_vm(format,arg,0);
}


