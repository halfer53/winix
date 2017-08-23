INCLUDE_DIR = ../include
INCLUDES = $(wildcard $(INCLUDE_DIR)/*.h $(INCLUDE_DIR)/sys/*.h)

PROGS = 
SOURCES = $(PROGS:=.c)
ASSEMBLY = $(PROGS:=.s)

OBJECT = $(PROGS:=.o) wramp_syscall.o ucontext.o debug.o

wramp: 
	$(MAKE) -C ansi
	$(MAKE) -C gen
	$(MAKE) -C posix
	$(MAKE) -C stdio
	$(MAKE) -C stdlib
	$(MAKE) -C syscall
	$(MAKE) -C util

depend:
	makedepend -I$(INCLUDE_DIR) $(SOURCES)

clean:
	$(MAKE) -C ansi clean
	$(MAKE) -C gen clean
	$(MAKE) -C posix clean
	$(MAKE) -C stdio clean
	$(MAKE) -C stdlib clean
	$(MAKE) -C syscall clean
	$(MAKE) -C util clean
	-rm -f $(OBJECT) $(ASSEMBLY)

.DELETE_ON_ERROR:
