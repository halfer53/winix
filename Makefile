
KERNEL_OBJS = winix/*.o kernel/*.o kernel/system/*.o

USER_LIB = ipc string util wramp_syscall ucontext atoi _sigset errno
USER_LIB_OBJS = $(addprefix lib/, $(USER_LIB:=.o))

WRAMP_LIMITS_OBJS = kernel/util/limits_head.o kernel/util/limits_tail.o
KERNEL_MAIN = kernel/main.s kernel/main.o 

REFORMAT = reformat_srec
GEN_BIN = gen_bin_code

all:
	-rm -f winix.srec
	$(MAKE) -C lib
	$(MAKE) shell
	$(MAKE) -C winix
	$(MAKE) -C kernel
	$(MAKE) -C user
	wlink -o winix.srec $(KERNEL_OBJS) $(WRAMP_LIMITS_OBJS) $(USER_LIB_OBJS)

install:
	$(MAKE) clean
	$(MAKE) all

clean:
	$(MAKE) -C kernel clean
	$(MAKE) -C lib clean
	$(MAKE) -C user clean
	$(MAKE) -C winix clean
	-rm -f winix.srec

stat:
	@echo "C Lines: "
	@find . -name "*.c" -exec cat {} \; | wc -l
	@echo "Header LoC: "
	@find . -name "*.h" -exec cat {} \; | wc -l
	@echo "Assembly LoC: "
	@find . -name "*.s" -exec cat {} \; | wc -l

shell:
	rm -f $(KERNEL_MAIN)
	wcc -S user/shell.c
	wasm shell.s
	wlink -o shell.srec shell.o lib/*.o
	[ ! -f $(REFORMAT).class ] && javac tools/$(REFORMAT).java || :
	java $(REFORMAT) shell.srec
	[ ! -f $(GEN_BIN) ] && gcc tools/$(GEN_BIN).c -o $(GEN_BIN) || :
	./$(GEN_BIN) shell.srec > include/shell_codes.c
	rm shell.srec

.DELETE_ON_ERROR:


