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

all:
	$(MAKE) -C tools
	$(MAKE) -C lib
	$(MAKE) -C user
	$(MAKE) shell
	$(MAKE) -C winix
	$(MAKE) -C kernel
	@wlink $(LDFLAGS) -o winix.srec $(L_HEAD) $(KERNEL_O) $(KLIB_O) $(L_TAIL)
	@echo "LD \t winix.srec"

release: 
	$(MAKE) clean
	$(MAKE) all CFLAGS=

clean:
	$(MAKE) -C kernel clean
	$(MAKE) -C lib clean
	$(MAKE) -C winix clean
	$(MAKE) -C user clean
	@-rm -f winix.srec
	@echo "RM \t winix.srec"

stat:
	@echo "C Lines: "
	@find . -type d -name "include" -prune -o -name "*.c"  -exec cat {} \; | wc -l
	@echo "Header LoC: "
	@find . -name "*.h" -exec cat {} \; | wc -l
	@echo "AS \tsembly LoC: "
	@find . -name "*.s" -exec cat {} \; | wc -l

shell:
	@rm -f $(KMAIN)
	@cp user/shell.srec .
	@java $(REFORMAT) shell.srec
	@./$(GEN_BIN) shell.srec > include/shell_codes.c
	@rm -f shell.srec shell.s shell.o
	@echo "LOAD\t SHELL"

test:
	gcc -D_GCC_DEBUG -I./include test.c winix/bitmap.c winix/mm.c

.DELETE_ON_ERROR:


