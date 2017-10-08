MAIN_DIR = ..
INCLUDE_DIR = $(MAIN_DIR)/include
INCLUDES = $(wildcard $(INCLUDE_DIR)/*.h $(INCLUDE_DIR)/sys/*.h )
LIBS_O = $(shell find ../lib -name "*.o")

SOURCES = 
ASSEMBLY = 
OBJECT = 

SH = shell
SHELL_COMPONENTS = shell parse shell_test
SHELL_PROGS = $(addprefix shell/, $(SHELL_COMPONENTS))
SHELL_SOURCES = $(SHELL_PROGS:=.c)
SHELL_ASSEMBLY = $(SHELL_PROGS:=.s)
SHELL_OBJECT = $(SHELL_PROGS:=.o)
SHELL_SREC = $(SH).srec

SOURCES += $(SHELL_SOURCES)
ASSEMBLY += $(SHELL_ASSEMBLY)
OBJECT += $(SHELL_OBJECT)

.PHONY : user shell

all: | depend user shell.srec
	
shell.srec: $(SHELL_OBJECT) $(LIBS_O) $(SH)/*.h
	$(Q)wlink $(LDFLAGS) -v -o $(SHELL_SREC) $(SHELL_OBJECT) $(LIBS_O) > \
								$(MAIN_DIR)/tools/kdbg_srec/shell.kdbg
	$(Q)java -cp $(MAIN_DIR)/tools reformat_srec $(SHELL_SREC)
	$(Q)./$(MAIN_DIR)/tools/gen_bin_code $(SHELL_SREC) > $(INCLUDE_DIR)/shell_bin.c

ifeq ($(KBUILD_VERBOSE),0)
	@echo "LD \t SHELL\nBIN\t SHELL"
endif

user: $(OBJECT)
$(ASSEMBLY): %.s: %.c
	$(Q)wcc $(CFLAGS) -S -N -I$(INCLUDE_DIR) -o $@ $<
ifeq ($(KBUILD_VERBOSE),0)
	@echo "CC \t $<"
endif
	
$(OBJECT): %.o: %.s
	$(Q)wasm $< -o $@
ifeq ($(KBUILD_VERBOSE),0)
	@echo "AS \t $<"
endif

foo: foo.c
	wcc -N -S -I../include -o foo.s foo.c
	wasm -o foo.o foo.s
	wlink -v -o foo.srec foo.o

depend:
	$(Q)makedepend -o.s -I$(INCLUDE_DIR) $(SOURCES)

clean:
	$(Q)-rm -f $(OBJECT) $(ASSEMBLY) $(SHELL_SREC)
ifeq ($(KBUILD_VERBOSE),0)
	@echo "RM \t USER"
endif

.DELETE_ON_ERROR:
# DO NOT DELETE

shell/shell.s: shell/shell.h ../include/unistd.h ../include/sys/types.h
shell/shell.s: ../include/sys/ipc.h ../include/signal.h
shell/shell.s: ../include/sys/unistd.h ../include/sys/wait.h
shell/shell.s: ../include/sys/syscall.h ../include/errno.h
shell/shell.s: ../include/stddef.h ../include/stdlib.h ../include/stdio.h
shell/shell.s: ../include/string.h ../include/debug.h ../include/ucontext.h
shell/shell.s: ../include/sys/ucontext.h ../include/sys/times.h shell/parse.h
shell/shell.s: ../include/ctype.h
shell/parse.s: ../include/stdlib.h ../include/stddef.h ../include/sys/types.h
shell/parse.s: shell/parse.h ../include/ctype.h
shell/shell_test.s: shell/shell.h ../include/unistd.h ../include/sys/types.h
shell/shell_test.s: ../include/sys/ipc.h ../include/signal.h
shell/shell_test.s: ../include/sys/unistd.h ../include/sys/wait.h
shell/shell_test.s: ../include/sys/syscall.h ../include/errno.h
shell/shell_test.s: ../include/stddef.h ../include/stdlib.h
shell/shell_test.s: ../include/stdio.h ../include/string.h ../include/debug.h
shell/shell_test.s: ../include/ucontext.h ../include/sys/ucontext.h
shell/shell_test.s: ../include/sys/times.h shell/parse.h ../include/ctype.h
