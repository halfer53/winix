# WINIX
A UNIX-style Operating System for the Waikato RISC Architecture Microprocessor (WRAMP)

NB Due to bugs in the kernel, the feature of redirection and multiple pipes for Bourne Shell have been moved to a separate project [TBash](https://github.com/halfer53/TBash)

## How to Compile
```cd Winix2```

```export PATH=`pwd`/tools/bin:$PATH```

```chmod +x tools/bin/*```

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

```kill [ -n signum ] pid```: send specified signum to specified process. If signum is not provided, SIGKIL is sent

```?```: show the list of commands available

### Testing commands

```test thread [ num ]```: Spawn the number of user threads specified by the parameter num, or 2 by default

```test malloc```: test malloc function

```test fork```: fork off a child shell process, parent shell wait until child shell exits or killed.

```test stack_overflow```: test stack overflow

```test float```: test floating point exception

```test alarm [ seconds ]```: set the alarm in number of seconds, or 1 by default

```test signal [ seconds ]```: set an alarm after specified seconds. Any system calls that are currently executing are interrupted (kgetc()) 

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
