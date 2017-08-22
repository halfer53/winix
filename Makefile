REFORMAT = reformat_srec
GEN_BIN = gen_bin_code

export CFLAGS = "-D_DEBUG"
RELEASE_FLAGS = ""

LIBS_O = $(shell find lib -name "*.o")

KLIB = ipc string util wramp_syscall ucontext stdlib/atoi
KLIB_O = $(addprefix lib/, $(KLIB:=.o))
L_HEAD = winix/limits/limits_head.o
L_TAIL = winix/limits/limits_tail.o
KERNEL_O = winix/*.o kernel/system/*.o kernel/*.o
KMAIN = kernel/main.s kernel/main.o 

all:
	$(MAKE) -C tools
	-rm -f winix.srec
	$(MAKE) -C lib
	$(MAKE) shell
	$(MAKE) -C winix
	$(MAKE) -C kernel
	wlink $(LDFLAGS) -o winix.srec $(L_HEAD) $(KERNEL_O) $(KLIB_O) $(L_TAIL)

release: 
	$(MAKE) clean
	$(MAKE) all CFLAGS=""

clean:
	$(MAKE) -C kernel clean
	$(MAKE) -C lib clean
	$(MAKE) -C winix clean
	-rm *.o *.s
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
	wcc -S user/shell_parse.c
	wcc -S user/shell_test.c
	wcc -S user/shell.c
	wasm shell.s
	wasm shell_test.s
	wasm shell_parse.s
	wlink -o shell.srec shell.o shell_test.o shell_parse.o $(LIBS_O)
	java $(REFORMAT) shell.srec
	./$(GEN_BIN) shell.srec > include/shell_codes.c
	-rm -f shell.srec shell.s shell.o

test:
	gcc -D_GCC_DEBUG -I./include test.c winix/bitmap.c winix/mm.c

.DELETE_ON_ERROR:


