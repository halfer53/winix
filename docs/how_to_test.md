# How to Test

Download [RexSimulator]()

Download [winix.srec](http://www.github.com)

Open ```RexSimulator```

Click ```Quick Load```

Select ```winix.srec```

## test fiber

type ```testfiber```

```
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

```

## test malloc

type ```testmalloc```

```
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
```

## test signal

press the ```red button``` indicated below to stop the program

then
Click ```Quick Load```
Select ```winix.srec```

type ```testsignal```

```
void sighandler(int signum){
	
	printf("\nSignal received, 2 second elapsed\n Child exit \n");
	sys_exit(0);
}

int test_signal(int argc, char **argv){
	int i;
	pid_t pid;
	pid_t fr;

	if((fr = fork()) == 0){

		signal(SIGALRM,sighandler);
		alarm(2);
		i = 10000;
		while(1){
			while(i--);
			putc('!');
			i = 10000;
		}
	}else{
		printf("parent waiting for child %d\n",fr);
		pid = wait(NULL);
		printf("parent awaken\n");
	}
	return 0;
}
```

## test file system

```cd fs```

```gcc *.c```

```./a.out```

```
	int fd = sys_open("/abc.txt",O_CREAT);
    sys_write(fd,"abc",4);
    sys_close(fd);

    char buf[4];
    fd = sys_open("/abc.txt",O_RDONLY);
    sys_read(fd,buf,4);
    sys_close(fd);

    printf("Got %s\n",buf);

```

## side notes

You can also play around with the shell by typing the folloing commands

```uptime```

```ps```

```exit```