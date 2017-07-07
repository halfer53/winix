# WINIX
A UNIX-style Operating System for the Waikato RISC Architecture Microprocessor (WRAMP)

## How to Compile
```cd Winix2```

```export PATH=`pwd`/tools/bin:$PATH```

```make```

## How to Run

Download the Rexsimulator from [Here](https://github.com/halfer53/rexsimulator/releases/tag/2.0.1)

Run ```Rexsimulator.exe```

Click ```Quick Load```, select ```winix.srec```

## List of Commands

alarm [ seconds ]: set the alarm in number of seconds, or 1 by default

thread [ num ]: Spawn the number of user threads specified by the parameter num, or 2 by default

malloc: test malloc function

kill [ -n signum ] pid: send specified signum to specified process. If signum is not provided, SIGKIL is sent

fork: fork off a child shell process, parent shell wait until child shell exits or killed.

exit: exit the shell program

uptime: shows the system uptime

[Adventure on Winix](https://github.com/halfer53/Winix2/blob/master/docs/demo.md)

## Credits
[Paul Monigatti](https://nz.linkedin.com/in/paulmonigatti)
