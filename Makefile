CC = wcc

TLIB = lib/*.o

KLIB = ipc string util wramp_syscall ucontext atoi errno
KLIB_OBJS = $(addprefix lib/, $(KLIB:=.o))
L_HEAD = kernel/util/limits_head.o
L_TAIL = kernel/util/limits_tail.o
KERNEL_OBJS = winix/*.o kernel/system/*.o kernel/*.o
KMAIN = kernel/main.s kernel/main.o 

REFORMAT = reformat_srec
GEN_BIN = gen_bin_code

all:
ifeq (, $(shell which $(CC)))
	$(error "Plz run: export PATH=`pwd`/tools/bin:$$PATH")
endif
	$(MAKE) -C tools
	-rm -f winix.srec
	$(MAKE) -C lib
	$(MAKE) shell
	$(MAKE) -C winix
	$(MAKE) -C kernel
	$(MAKE) -C user
	wlink -o winix.srec $(L_HEAD) $(KERNEL_OBJS) $(KLIB_OBJS) $(L_TAIL)

debug:
	$(MAKE) clean
	$(MAKE) -C lib debug
	$(MAKE) -C winix debug
	$(MAKE) -C kernel debug
	$(MAKE) -C user debug
	wlink -o winix.srec $(KLIMITS) $(KERNEL_OBJS) $(LIB_OBJS)

clean:
	$(MAKE) -C tools clean
	$(MAKE) -C kernel clean
	$(MAKE) -C lib clean
	$(MAKE) -C user clean
	$(MAKE) -C winix clean
	-rm -f winix.srec

stat:
	@echo "C Lines: "
	@find . -type d -name "include" -prune -o -name "*.c"  -exec cat {} \; | wc -l
	@echo "Header LoC: "
	@find . -name "*.h" -exec cat {} \; | wc -l
	@echo "Assembly LoC: "
	@find . -name "*.s" -exec cat {} \; | wc -l

shell:
	-rm -f $(KMAIN)
	wcc -S user/shell.c
	wasm shell.s
	wlink -o shell.srec shell.o lib/*.o
	java $(REFORMAT) shell.srec
	./$(GEN_BIN) shell.srec > include/shell_codes.c
	-rm -f shell.srec

test:
	gcc -D_GCC_DEBUG -I./include test.c winix/bitmap.c winix/mm.c

.DELETE_ON_ERROR:


