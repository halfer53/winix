objs = kernel/winix/*.o kernel/*.o user/*.o lib/ipc.o lib/string.o lib/util.o lib/syscall.o lib/wramp_syscall.o

all:
	$(MAKE) -C lib
	$(MAKE) -C kernel
	$(MAKE) -C user
	wlink -o winix.srec kernel/util/limits_head.o $(objs) kernel/util/limits_tail.o

clean:
	$(MAKE) -C kernel clean
	$(MAKE) -C lib clean
	$(MAKE) -C user clean
	rm winix.srec

stat:
	@echo "C Lines: "
	@find . -name "*.c" -exec cat {} \; | wc -l
	@echo "Header LoC: "
	@find . -name "*.h" -exec cat {} \; | wc -l
	@echo "Assembly LoC: "
	@find . -name "*.s" -exec cat {} \; | wc -l

shell:
	$(MAKE) -C lib
	cp user/shell.c .
	wcc -S shell.c
	wasm shell.s
	wlink -o shell.srec shell.o lib/*.o
	java reformat_srec shell.srec
	rm shell.c
	rm shell.o
	rm shell.s
	gcc gen_bin_code.c
	./a.out shell.srec > include/shell_codes.c
	rm shell.srec
	$(MAKE) all

test:
	wcc -S test.c
	wasm test.s
	wlink -o test.srec test.o lib/stdio.o lib/ucontext.o
	rm test.o
	rm test.s

.DELETE_ON_ERROR:
