REFORMAT = reformat_srec
GEN_BIN = gen_bin_code

export CFLAGS = -D_DEBUG
RELEASE_FLAGS = 

# List of user libraries used by the kernel
KLIB = syscall/wramp_syscall syscall/ipc_receive ansi/string util/util \
		gen/ucontext stdlib/atoi util/debug

L_HEAD = winix/limits/limits_head.o
L_TAIL = winix/limits/limits_tail.o
KERNEL_O = winix/*.o kernel/system/*.o kernel/*.o
KMAIN = kernel/main.s kernel/main.o 
KLIB_O = $(addprefix lib/, $(KLIB:=.o))

# Check if V options is set by user, if V=1, debug mode is set
# e.g. make V=1 produces all the commands being executed through
# the building process
ifeq ("$(origin V)", "command line")
	  KBUILD_VERBOSE = $(V)
endif
ifndef KBUILD_VERBOSE
	  KBUILD_VERBOSE = 0
endif

ifeq ($(KBUILD_VERBOSE),1)
	  quiet =
	  Q =
else
	MAKEFLAGS += --no-print-directory
	  quiet=quiet_
	  Q = @
endif

export KBUILD_VERBOSE
export Q
export quiet

all:
	$(Q)-rm -f $(KMAIN)
	$(Q)$(MAKE) -C tools
	$(Q)$(MAKE) -C lib
	$(Q)$(MAKE) -C user
	$(Q)$(MAKE) -C winix
	$(Q)$(MAKE) -C init
	$(Q)$(MAKE) -C kernel
	$(Q)wlink $(LDFLAGS) -Ttext 1024 -v -o winix.srec \
			$(L_HEAD) $(KERNEL_O) $(KLIB_O) $(L_TAIL) > tools/kdbg_srec/winix.kdbg
ifeq ($(KBUILD_VERBOSE),0)
	@echo "LD \t winix.srec"
endif

d-verbose:
	$(MAKE) all CFLAGS=-D_DEBUG=2

release: 
	$(MAKE) clean
	$(MAKE) all CFLAGS=$(RELEASE_FLAGS)

clean:
	$(Q)$(MAKE) -C kernel clean
	$(Q)$(MAKE) -C lib clean
	$(Q)$(MAKE) -C winix clean
	$(Q)$(MAKE) -C user clean
	$(Q)$(MAKE) -C init clean
ifeq ($(KBUILD_VERBOSE),0)
	@echo "RM \t shell.srec"
	@echo "RM \t winix.srec"
endif

stat:
	@echo "C Lines: "
	@find . -type d -name "include" -prune -o -name "*.c"  -exec cat {} \; | wc -l
	@echo "Header LoC: "
	@find . -name "*.h" -exec cat {} \; | wc -l
	@echo "Assembly LoC: "
	@find . -name "*.s" -exec cat {} \; | wc -l

test:
	gcc -D_GCC_DEBUG -I./include test.c winix/bitmap.c winix/mm.c

.DELETE_ON_ERROR:


