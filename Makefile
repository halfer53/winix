objs = kernel/winix/*.o kernel/*.o user/*.o lib/ipc.o lib/string.o lib/util.o lib/syscall.o lib/wramp_syscall.o

all:
	$(MAKE) -C lib
	$(MAKE) shell
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
	cp user/shell.c .
	wcc -S shell.c
	wasm shell.s
	wlink -o shell.srec shell.o lib/string.o lib/stdio.o lib/syscall.o lib/ipc.o lib/wramp_syscall.o lib/stdlib.o
	java reformat_srec shell.srec
	rm shell.c
	rm shell.o
	rm shell.s
	gcc gen_bin_code.c
	./a.out shell.srec > include/exec_codes.c
	#rm shell.srec

printf:
	cp user/printf.c .
	wcc -S printf.c
	wasm printf.s
	wlink -o printf.srec printf.o lib/stdio.o lib/syscall.o lib/ipc.o lib/wramp_syscall.o
	java reformat_srec printf.srec
	rm printf.c
	rm printf.o
	rm printf.s

.DELETE_ON_ERROR:
