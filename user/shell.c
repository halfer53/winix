/**
 * Simple shell for WINIX.
 *
 * Revision History:
 *  2016-09-19		Paul Monigatti			Original
 *  2016-11-20		Bruce Tan			Modified
 **/

#include <sys/syscall.h>
#include <stdio.h>
#include <stddef.h>
#include <sys/types.h>
#include <size.h>
#include <stdlib.h>
#include <ucontext.h>
#include <signal.h>

#define BUF_LEN		100

//Prototypes
int ps(int argc, char **argv);
int uptime(int argc, char **argv);
int shutdown(int argc, char **argv);
int exit(int argc, char **argv);
int testmalloc(int argc, char **argv);
int test_signal(int argc, char **argv);
int test_fiber(int argc, char **argv);
int test_alarm(int argc, char **argv);
int generic(int argc, char **argv);

//Input buffer & tokeniser
static char buf[BUF_LEN];
static char *tokens[BUF_LEN / 2];

//Maps strings to function pointers
struct cmd {
	char *name;
	int (*handle)(int argc, char **argv);
};

//Command handling
struct cmd commands[] = {
	{ "uptime", uptime },
	{ "shutdown", shutdown },
	{ "exit", exit },
	{ "ps", ps },
	{ "testmalloc", testmalloc},
	{ "testsignal", test_signal},
	{ "testfiber", test_fiber},
	{ "testalarm", test_alarm},
	{ NULL, generic }
};
//TODO: ps/uptime/shutdown should be moved to separate programs.

/**
 * Returns true if c is a printable character.
 **/
int isPrintable(int c) {
	return ('!' <= c && c <= '~');
}

void alarm_handler(int signum){
	int *ni = 0;
	*ni = 0x123456;
	printf("\nalarm timer received\n");
	printf("WINIX>%s",buf);
}

int test_alarm(int argc, char **argv){
	signal(SIGALRM,alarm_handler);
	alarm(1);
	return 0;
}

void sighandler(int signum){
	
	printf("\nSignal received, 1 second elapsed\nChild exit \n");
	sys_exit(0);
}

int test_signal(int argc, char **argv){
	int i;
	pid_t pid;
	pid_t fr;

	if((fr = fork()) == 0){
		signal(SIGALRM,sighandler);
		alarm(1);
		i = 10000;
		while(1){
			while(i--);
			putc('!');
			i = 10000;
		}
	}else{
		printf("parent waiting for child %d\n",fr);
		pid = wait(NULL);
		printf("parent awaken by child %d\n",pid);
	}
	return 0;
}

ucontext_t mcontext,fcontext,econtext;
int x = 0;

void func(int arg) {

  printf("Fiber %d\n",arg);
  x++;
  printf("Fiber %d returning to main\n",arg);
  setcontext(&mcontext);
}

int test_fiber(int argc, char **argv){
	int  value = 3;
	getcontext(&fcontext);
	if ((fcontext.ss_sp = (uint32_t *) malloc(1000)) != NULL) {
		fcontext.ss_sp += 1000;
		fcontext.ss_size = 1000;
		fcontext.ss_flags = 0;
		makecontext(&fcontext,func,1,1);
	}

	if ((econtext.ss_sp = (uint32_t *) malloc(1000)) != NULL) {
		econtext.ss_sp += 1000;
		econtext.ss_size = 1000;
		econtext.ss_flags = 0;
		makecontext(&econtext,func,1,2);
	}

	printf("context has been built\n");
	swapcontext(&mcontext,&fcontext);
	swapcontext(&mcontext,&econtext);
	if (!x) {
		printf("incorrect return from swapcontext");
	}
	else {
		printf("returned from function\n");
	}
	return 0;
}


int testmalloc(int argc, char **argv){
	
	void *p0 = malloc(512);
	  void *p1 = malloc(512);
	  void *p2 = malloc(1024);
	  void *p3 = malloc(512);
	  void *p4 = malloc(1024);
	  void *p5 = malloc(2048);
	  void *p6 = malloc(512);
	  void *p7 = malloc(1024);
	  void *p8 = malloc(512);
	  void *p9 = malloc(1024);
	  block_overview();
	  free(p5);
	  free(p6);
	  free(p2);
	  free(p8);
	  block_overview();
  
  return 0;
}

int ps(int argc, char **argv){
	return sys_process_overview();
}

/**
 * Prints the system uptime
 **/
int uptime(int argc, char **argv) {
	int ticks, days, hours, minutes, seconds;

	ticks = sys_uptime();
	seconds = ticks / 60; //TODO: define tick rate somewhere
	minutes = seconds / 60;
	hours = minutes / 60;
	days = hours / 24;

	seconds %= 60;
	minutes %= 60;
	hours %= 24;
	// ticks %= 100;

	printf("Uptime is %dd %dh %dm %d.%ds, total ticks: %d\r\n", days, hours, minutes, seconds, ticks%100, ticks);
	return 0;
}

/**
 * Shuts down OS.
 **/
int shutdown(int argc, char **argv) {
	printf("Placeholder for SHUTDOWN\r\n");
	return 0;
}

/**
 * Exits the terminal.
 **/
int exit(int argc, char **argv) {
	printf("Bye!\r\n");
	return sys_exit(0);
}

/**
 * Handles any unknown command.
 **/
int generic(int argc, char **argv) {
	//Quietly ignore empty file paths
	if(argc == 0)
		return 0;

		if (strcmp("exec",argv[0]) == 0) {
			printf("please drag the srec file onto this windows\n" );
			return exec();
		}else if (strcmp("fork",argv[0]) == 0) {
			int forkid = 0;
			forkid = fork();
			if (forkid != 0)
			{
				printf("I am parent\n");
			}else{
				printf("I am child\n");
			}
			return 0;
		}
	printf("Unknown command '%s'\r\n", argv[0]);
	return -1;
}



void main() {
	int i, j;
	int argc;
	char ret;
	char *c;
	struct cmd *handler = NULL;
	
	// alarm_handler(0);
	// test_alarm(0,NULL);
	while(1) {
		printf("WINIX> ");
		i=0;
		//Read line from terminal
		while( i < BUF_LEN - 2) {

			ret = getc(); 	//read
			
			if(ret == -1)
				break;

			if(ret == '\r')  //test for end
				break;
				
			buf[i] = ret;
			if ((int)ret == 8) { //backspace

				if (i != 0) {
					putc(ret);
					i--;
				}
				continue;
			}
			i++;
			putc(ret); 		//echo
		}
		buf[++i] = '\0';
		putc('\n');

		//Tokenise command
		//TODO: proper parsing of arguments
		argc = 0;
		c = buf;
		while(*c) {

			//Skip over non-alphanumeric characters
			while(*c && !isPrintable(*c)){
				c++;
			}


			//Add new token
			if(*c != '\0') {
				tokens[argc++] = c;
			}

			//Skip over alphanumeric characters
			while(*c && isPrintable(*c)){
				c++;
			}


			if(*c != '\0') {
				*c++ = '\0';
			}
		}


		//Decode command
		handler = commands;
		while(handler->name != NULL && strcmp(tokens[0], handler->name)) {
			handler++;
		}

		//Run it
		handler->handle(argc, tokens);
	}
	sys_exit(0);
}
