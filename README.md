# WINIX

TODO
- change rbase to use mem_start instead
- add mem_start, text_size, data_size, bss_size to struct proc

A UNIX-style Operating System for the Waikato RISC Architecture Microprocessor (WRAMP)

NB Due to bugs in the kernel, the feature of redirection and multiple pipes for Bourne Shell have been moved to a separate project [TBash](https://github.com/halfer53/TBash)

## Supported System Call

```
Name        Syscall Number

TIMES           1
EXIT            2
FORK            3
VFORK           4
EXECVE          5
BRK             6
ALARM           7
SIGACTION       8
SIGRET          9
WAITPID         10
KILL            11
GETPID          12
WINFO           13
GETC            14
WINIX_DPRINTF          15
SYSCONF         16
SIGSUSPEND      17
SIGPENDING      18
SIGPROCMASK     19
SETPGID         20
GETPGID         21
```

## Features

 - Kernel Memory Management: visual memory, page mapping, kmalloc() kfree()
 - Process Management: Kernel thread, process creation
 - Interrupt Handler
 - A Bourne shell 
 - User memory management: malloc() free()
 - User coroutine 
 - Stacktrace dump, Segfault analysis

## Prerequisite

```sudo apt-get install xutils-dev```

```cd winix```

```chmod +x tools/bin/*```

```export PATH=`pwd`/tools/bin:$PATH```

## How to Compile

```make```

## How to Run

Download the Rexsimulator from [Here](https://github.com/halfer53/rexsimulator/releases/tag/2.0.1)

Run ```Rexsimulator.exe```

Click ```Quick Load```, select ```winix.srec```

## List of Commands

### Built_in commands

```ps```: shows the list of processes running in the system, with relevant info

```free```: shows the current system memory info

```pid```: shows the current shell's pid

```exit```: exit the shell program

```uptime```: shows the system uptime

```kill [ -s signum ] pid```: send specified signum to specified process. If signum is not provided, SIGKIL is sent

```bash```: fork off a new bash shell.

```printenv```: print all the environment variables

```printheap```: print the heap memory pools

```help```: show the list of commands available

### Testing commands

```test thread [ num ]```: Spawn the number of user threads specified by the parameter num, or 2 by default

```test malloc```: test malloc function

```test stack```: test stack overflow

```test float```: test floating point exception

```test alarm [ seconds ]```: set the alarm in number of seconds, or 1 by default

```test signal [ seconds ]```: set an alarm after specified seconds. Any system calls that are currently executing are interrupted (kgetc()) 

```test vfork```: test vfork

```test deadlock```: test deadlock

```test ipc```: test ipc

## Demo

[Demo](https://github.com/halfer53/Winix2/blob/master/Documentations/demo.md)

## Debug tips

### Debug Makefile

Similar to linux kbuild, **Winix** supports verbose option, you can debug Makefile by setting verbose to 1

```make V=1```

### Debug Kernel

```kprintf()``` is your friend. 

```debug_syscall();```: Print all the system call from user space.

```debug_ipc(int val);```: Print IPC messages in the system, you can set the limit on the number of maximum ipc messages to be displayed. e.g. if val is set to 10, then only the next 10 ipc messages will be printed out.

```debug_scheduling(int val)```: Print the process that the scheduling algorithms choose during each clock interrupt. val indicates the number of maximum messages to be printed.

## Run File System

Currently the file system is not integrated into the OS yet, but you can still test it as a standalone process

```
cd fs
gcc *.c
./a.out
```

## Credits
[Paul Monigatti](https://nz.linkedin.com/in/paulmonigatti)
