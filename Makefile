objs = winix/*.o kernel/*.o lib/ipc.o lib/string.o lib/util.o lib/wramp_syscall.o
REFORMAT = tools/reformat_srec
GEN_BIN = tools/gen_bin_code

all:
	$(MAKE) -C lib
	$(MAKE) -C winix
	$(MAKE) -C kernel
	$(MAKE) -C user
	wlink -o winix.srec kernel/util/limits_head.o $(objs) kernel/util/limits_tail.o

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
	$(MAKE) clean
	$(MAKE) -C lib
	wcc -S user/shell.c
	wasm shell.s
	wlink -o shell.srec shell.o lib/*.o
	[ ! -f reformat_srec.class ] && javac tools/reformat_srec.java || :
	java reformat_srec shell.srec
	[ ! -f gen_bin_code ] && gcc tools/gen_bin_code.c || :
	./gen_bin_code shell.srec > include/shell_codes.c
	$(MAKE) all

test:
	test -f myApp && echo File does exist

.DELETE_ON_ERROR:


