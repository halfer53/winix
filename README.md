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

```alarm [ seconds ]```: set the alarm in number of seconds, or 1 by default

```thread [ num ]```: Spawn the number of user threads specified by the parameter num, or 2 by default

```malloc```: test malloc function

```kill [ -n signum ] pid```: send specified signum to specified process. If signum is not provided, SIGKIL is sent

```fork```: fork off a child shell process, parent shell wait until child shell exits or killed.

```exit```: exit the shell program

```uptime```: shows the system uptime

```signal [ seconds ]```: set an alarm after specified seconds. Any system calls that are currently executing are interrupted (kgetc()) 

```ps```: shows the list of processes running in the system, with relevant info

```meminfo```: shows the current system memory info

```pid```: shows the current shell's pid

## Demo

[Demo](https://github.com/halfer53/Winix2/blob/master/Documentations/demo.md)

## Run File System

Currently the file system is not integrated into the OS yet, but you can still test it as a standalone process

```
cd fs
gcc *.c
./a.out
```

## Credits
[Paul Monigatti](https://nz.linkedin.com/in/paulmonigatti)
