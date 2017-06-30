# WINIX
A UNIX-style Operating System for the Waikato RISC Architecture Microprocessor (WRAMP)[Paper](https://www.ncsu.edu/wcae/ISCA2002/submissions/pearson.pdf)

# Install
Since the compiler for this project is not open source (not yet), you may not compile this on your own. 

But you can stil run the compiled code on the [Simulator](https://sourceforge.net/projects/rexsimulator/). Once the simulator is run, open up Serial Port 1, and type ```load```, then drag ```winix.srec``` on the the serial port 1. Wait for a minute until the OS is loaded, then type ```run```

# TODO
- [ ] fix bugs for sbrk() and brk() after fork()
- [ ] sigreturn() is blocked by previous getc()

# Credits
[Paul Monigatti](https://nz.linkedin.com/in/paulmonigatti)