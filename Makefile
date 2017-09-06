REFORMAT = reformat_srec
GEN_BIN = gen_bin_code

export CFLAGS = -D_DEBUG
RELEASE_FLAGS = 

KLIB = syscall/ipc ansi/string util/util syscall/wramp_syscall gen/ucontext stdlib/atoi
KLIB_O = $(addprefix lib/, $(KLIB:=.o))
L_HEAD = winix/limits/limits_head.o
L_TAIL = winix/limits/limits_tail.o
KERNEL_O = winix/*.o kernel/system/*.o kernel/*.o
KMAIN = kernel/main.s kernel/main.o 

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
	$(Q)$(MAKE) -C tools
	$(Q)$(MAKE) -C lib
	$(Q)$(MAKE) -C user
	$(Q)$(MAKE) -C winix
	$(Q)$(MAKE) init
	$(Q)$(MAKE) shell
	$(Q)$(MAKE) -C kernel
	$(Q)wlink $(LDFLAGS) -o winix.srec $(L_HEAD) $(KERNEL_O) $(KLIB_O) $(L_TAIL)
ifeq ($(KBUILD_VERBOSE),0)
	@echo "LD \t winix.srec"
endif

release: 
	$(MAKE) clean
	$(MAKE) all CFLAGS=$(RELEASE_FLAGS)

clean:
	$(Q)$(MAKE) -C kernel clean
	$(Q)$(MAKE) -C lib clean
	$(Q)$(MAKE) -C winix clean
	$(Q)$(MAKE) -C user clean
	$(Q)$(MAKE) -C init clean
	$(Q)-rm -f shell.srec
	$(Q)-rm -f winix.srec
ifeq ($(KBUILD_VERBOSE),0)
	@echo "RM \t shell.srec"
	@echo "RM \t winix.srec"
endif

stat:
	@echo "C Lines: "
	@find . -type d -name "include" -prune -o -name "*.c"  -exec cat {} \; | wc -l
	@echo "Header LoC: "
	@find . -name "*.h" -exec cat {} \; | wc -l
	@echo "AS \tsembly LoC: "
	@find . -name "*.s" -exec cat {} \; | wc -l

shell: user/shell.c user/shell_test.c user/shell_parse.c
	$(Q)-rm -f $(KMAIN)
	$(Q)cp user/shell.srec .
	$(Q)java $(REFORMAT) shell.srec
	$(Q)./$(GEN_BIN) shell.srec > include/shell_codes.c
	$(Q)rm -f shell.srec
	$(Q)echo "BIN\t SHELL"

init: init/init.c
	$(Q)$(MAKE) -C init
	$(Q)-rm -f $(KMAIN)
	$(Q)cp init/init.srec .
	$(Q)java $(REFORMAT) init.srec
	$(Q)./$(GEN_BIN) init.srec > include/init_codes.c
	$(Q)rm -f init.srec
	$(Q)echo "BIN\t INIT"

test:
	gcc -D_GCC_DEBUG -I./include test.c winix/bitmap.c winix/mm.c

.DELETE_ON_ERROR:


