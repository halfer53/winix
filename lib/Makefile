INCLUDE_DIR = ../include
INCLUDES = $(wildcard $(INCLUDE_DIR)/*.h $(INCLUDE_DIR)/sys/*.h)

MODULES := ansi gen posix stdio stdlib syscall util
SOURCES = $(foreach sdir, $(MODULES), $(wildcard $(sdir)/*.c))
ASSEMBLY = $(SOURCES:.c=.s)
OBJECT = $(SOURCES:.c=.o) syscall/wramp_syscall.o util/debug.o gen/ucontext.o\
							syscall/vfork.o

.PHONY: lib depend

all: | depend lib

lib: $(OBJECT)
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

depend:
	$(Q)makedepend -o.s -I$(INCLUDE_DIR) $(SOURCES)

clean:
	$(Q)-rm -f $(OBJECT) $(ASSEMBLY)
ifeq ($(KBUILD_VERBOSE),0)
	@echo "RM \t LIB"
endif

.DELETE_ON_ERROR:
# DO NOT DELETE

ansi/raise.s: ../include/lib.h ../include/errno.h ../include/sys/types.h
ansi/raise.s: ../include/sys/syscall.h ../include/sys/ipc.h
ansi/raise.s: ../include/signal.h ../include/stddef.h ../include/stdlib.h
ansi/raise.s: ../include/stdio.h ../include/string.h ../include/const.h
ansi/env.s: ../include/lib.h ../include/errno.h ../include/sys/types.h
ansi/env.s: ../include/sys/syscall.h ../include/sys/ipc.h ../include/signal.h
ansi/env.s: ../include/stddef.h ../include/stdlib.h ../include/stdio.h
ansi/env.s: ../include/string.h ../include/const.h
ansi/setenv.s: ../include/lib.h ../include/errno.h ../include/sys/types.h
ansi/setenv.s: ../include/sys/syscall.h ../include/sys/ipc.h
ansi/setenv.s: ../include/signal.h ../include/stddef.h ../include/stdlib.h
ansi/setenv.s: ../include/stdio.h ../include/string.h ../include/const.h
ansi/abort.s: ../include/lib.h ../include/errno.h ../include/sys/types.h
ansi/abort.s: ../include/sys/syscall.h ../include/sys/ipc.h
ansi/abort.s: ../include/signal.h ../include/stddef.h ../include/stdlib.h
ansi/abort.s: ../include/stdio.h ../include/string.h ../include/const.h
ansi/errlist.s: ../include/lib.h ../include/errno.h ../include/sys/types.h
ansi/errlist.s: ../include/sys/syscall.h ../include/sys/ipc.h
ansi/errlist.s: ../include/signal.h ../include/stddef.h ../include/stdlib.h
ansi/errlist.s: ../include/stdio.h ../include/string.h ../include/const.h
ansi/putenv.s: ../include/stdlib.h ../include/stddef.h ../include/sys/types.h
ansi/putenv.s: ../include/string.h
ansi/rand.s: ../include/stdlib.h ../include/stddef.h ../include/sys/types.h
ansi/string.s: ../include/lib.h ../include/errno.h ../include/sys/types.h
ansi/string.s: ../include/sys/syscall.h ../include/sys/ipc.h
ansi/string.s: ../include/signal.h ../include/stddef.h ../include/stdlib.h
ansi/string.s: ../include/stdio.h ../include/string.h ../include/const.h
ansi/getenv.s: ../include/stdlib.h ../include/stddef.h ../include/sys/types.h
gen/makecontext.s: ../include/lib.h ../include/errno.h ../include/sys/types.h
gen/makecontext.s: ../include/sys/syscall.h ../include/sys/ipc.h
gen/makecontext.s: ../include/signal.h ../include/stddef.h
gen/makecontext.s: ../include/stdlib.h ../include/stdio.h ../include/string.h
gen/makecontext.s: ../include/const.h ../include/ucontext.h
gen/makecontext.s: ../include/sys/ucontext.h
posix/_sigset.s: ../include/errno.h ../include/signal.h
posix/_sigset.s: ../include/sys/types.h
stdio/stdio.s: ../include/lib.h ../include/errno.h ../include/sys/types.h
stdio/stdio.s: ../include/sys/syscall.h ../include/sys/ipc.h
stdio/stdio.s: ../include/signal.h ../include/stddef.h ../include/stdlib.h
stdio/stdio.s: ../include/stdio.h ../include/string.h ../include/const.h
stdlib/stdlib.s: ../include/lib.h ../include/errno.h ../include/sys/types.h
stdlib/stdlib.s: ../include/sys/syscall.h ../include/sys/ipc.h
stdlib/stdlib.s: ../include/signal.h ../include/stddef.h ../include/stdlib.h
stdlib/stdlib.s: ../include/stdio.h ../include/string.h ../include/const.h
stdlib/errno.s: ../include/lib.h ../include/errno.h ../include/sys/types.h
stdlib/errno.s: ../include/sys/syscall.h ../include/sys/ipc.h
stdlib/errno.s: ../include/signal.h ../include/stddef.h ../include/stdlib.h
stdlib/errno.s: ../include/stdio.h ../include/string.h ../include/const.h
syscall/times.s: ../include/lib.h ../include/errno.h ../include/sys/types.h
syscall/times.s: ../include/sys/syscall.h ../include/sys/ipc.h
syscall/times.s: ../include/signal.h ../include/stddef.h ../include/stdlib.h
syscall/times.s: ../include/stdio.h ../include/string.h ../include/const.h
syscall/times.s: ../include/sys/times.h
syscall/execve.s: ../include/lib.h ../include/errno.h ../include/sys/types.h
syscall/execve.s: ../include/sys/syscall.h ../include/sys/ipc.h
syscall/execve.s: ../include/signal.h ../include/stddef.h ../include/stdlib.h
syscall/execve.s: ../include/stdio.h ../include/string.h ../include/const.h
syscall/getchar.s: ../include/lib.h ../include/errno.h ../include/sys/types.h
syscall/getchar.s: ../include/sys/syscall.h ../include/sys/ipc.h
syscall/getchar.s: ../include/signal.h ../include/stddef.h
syscall/getchar.s: ../include/stdlib.h ../include/stdio.h ../include/string.h
syscall/getchar.s: ../include/const.h
syscall/printf.s: ../include/lib.h ../include/errno.h ../include/sys/types.h
syscall/printf.s: ../include/sys/syscall.h ../include/sys/ipc.h
syscall/printf.s: ../include/signal.h ../include/stddef.h ../include/stdlib.h
syscall/printf.s: ../include/stdio.h ../include/string.h ../include/const.h
syscall/_syscall.s: ../include/lib.h ../include/errno.h
syscall/_syscall.s: ../include/sys/types.h ../include/sys/syscall.h
syscall/_syscall.s: ../include/sys/ipc.h ../include/signal.h
syscall/_syscall.s: ../include/stddef.h ../include/stdlib.h
syscall/_syscall.s: ../include/stdio.h ../include/string.h ../include/const.h
syscall/getpid.s: ../include/lib.h ../include/errno.h ../include/sys/types.h
syscall/getpid.s: ../include/sys/syscall.h ../include/sys/ipc.h
syscall/getpid.s: ../include/signal.h ../include/stddef.h ../include/stdlib.h
syscall/getpid.s: ../include/stdio.h ../include/string.h ../include/const.h
syscall/_exit.s: ../include/lib.h ../include/errno.h ../include/sys/types.h
syscall/_exit.s: ../include/sys/syscall.h ../include/sys/ipc.h
syscall/_exit.s: ../include/signal.h ../include/stddef.h ../include/stdlib.h
syscall/_exit.s: ../include/stdio.h ../include/string.h ../include/const.h
syscall/sigaction.s: ../include/lib.h ../include/errno.h
syscall/sigaction.s: ../include/sys/types.h ../include/sys/syscall.h
syscall/sigaction.s: ../include/sys/ipc.h ../include/signal.h
syscall/sigaction.s: ../include/stddef.h ../include/stdlib.h
syscall/sigaction.s: ../include/stdio.h ../include/string.h
syscall/sigaction.s: ../include/const.h
syscall/wait.s: ../include/lib.h ../include/errno.h ../include/sys/types.h
syscall/wait.s: ../include/sys/syscall.h ../include/sys/ipc.h
syscall/wait.s: ../include/signal.h ../include/stddef.h ../include/stdlib.h
syscall/wait.s: ../include/stdio.h ../include/string.h ../include/const.h
syscall/kill.s: ../include/lib.h ../include/errno.h ../include/sys/types.h
syscall/kill.s: ../include/sys/syscall.h ../include/sys/ipc.h
syscall/kill.s: ../include/signal.h ../include/stddef.h ../include/stdlib.h
syscall/kill.s: ../include/stdio.h ../include/string.h ../include/const.h
syscall/ipc.s: ../include/sys/ipc.h ../include/signal.h
syscall/ipc.s: ../include/sys/types.h ../include/stddef.h ../include/errno.h
syscall/sysconf.s: ../include/lib.h ../include/errno.h ../include/sys/types.h
syscall/sysconf.s: ../include/sys/syscall.h ../include/sys/ipc.h
syscall/sysconf.s: ../include/signal.h ../include/stddef.h
syscall/sysconf.s: ../include/stdlib.h ../include/stdio.h ../include/string.h
syscall/sysconf.s: ../include/const.h
syscall/alarm.s: ../include/lib.h ../include/errno.h ../include/sys/types.h
syscall/alarm.s: ../include/sys/syscall.h ../include/sys/ipc.h
syscall/alarm.s: ../include/signal.h ../include/stddef.h ../include/stdlib.h
syscall/alarm.s: ../include/stdio.h ../include/string.h ../include/const.h
syscall/brk_sbrk.s: ../include/lib.h ../include/errno.h
syscall/brk_sbrk.s: ../include/sys/types.h ../include/sys/syscall.h
syscall/brk_sbrk.s: ../include/sys/ipc.h ../include/signal.h
syscall/brk_sbrk.s: ../include/stddef.h ../include/stdlib.h
syscall/brk_sbrk.s: ../include/stdio.h ../include/string.h ../include/const.h
syscall/sys_winfo.s: ../include/lib.h ../include/errno.h
syscall/sys_winfo.s: ../include/sys/types.h ../include/sys/syscall.h
syscall/sys_winfo.s: ../include/sys/ipc.h ../include/signal.h
syscall/sys_winfo.s: ../include/stddef.h ../include/stdlib.h
syscall/sys_winfo.s: ../include/stdio.h ../include/string.h
syscall/sys_winfo.s: ../include/const.h
syscall/fork.s: ../include/lib.h ../include/errno.h ../include/sys/types.h
syscall/fork.s: ../include/sys/syscall.h ../include/sys/ipc.h
syscall/fork.s: ../include/signal.h ../include/stddef.h ../include/stdlib.h
syscall/fork.s: ../include/stdio.h ../include/string.h ../include/const.h
util/util.s: ../include/util.h
