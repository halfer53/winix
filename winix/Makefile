INCLUDE_DIR = ../include
INCLUDES = $(wildcard $(INCLUDE_DIR)/*.h $(INCLUDE_DIR)/winix/*.h \
			$(INCLUDE_DIR)/sys/*.h $(INCLUDE_DIR)/kernel/*.h)

PROGS = mem_alloc mm bitmap sys_stdio wini_ipc kdebug timer sigsend util\

FOO = *.c
SOURCES = $(PROGS:=.c)
ASSEMBLY = $(PROGS:=.s)
OBJECT = $(PROGS:=.o) kwramp.o limits/limits_head.o limits/limits_tail.o

.PHONY: depend winix
all: | depend winix

winix: $(OBJECT)
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
	@echo "RM \t WINIX"
endif

.DELETE_ON_ERROR:
# DO NOT DELETE

mem_alloc.s: ../include/kernel/kernel.h ../include/sys/ipc.h
mem_alloc.s: ../include/signal.h ../include/sys/types.h
mem_alloc.s: ../include/sys/syscall.h ../include/sys/unistd.h
mem_alloc.s: ../include/stddef.h ../include/util.h ../include/const.h
mem_alloc.s: ../include/errno.h ../include/stdbool.h ../include/curses.h
mem_alloc.s: ../include/debug.h ../include/string.h ../include/stdio.h
mem_alloc.s: ../include/winix/comm.h ../include/winix/compiler.h
mem_alloc.s: ../include/winix/type.h ../include/winix/page.h
mem_alloc.s: ../include/winix/util.h ../include/winix/kdebug.h
mem_alloc.s: ../include/winix/bitmap.h ../include/winix/mm.h
mem_alloc.s: ../include/kernel/proc.h ../include/winix/timer.h
mem_alloc.s: ../include/winix/kwramp.h ../include/winix/gfp.h
mem_alloc.s: ../include/winix/sys_stdio.h ../include/winix/wini_ipc.h
mem_alloc.s: ../include/winix/rex.h ../include/winix/mem_alloc.h
mem_alloc.s: ../include/kernel/system.h ../include/winix/syscall_ctx.h
mem_alloc.s: ../include/ucontext.h ../include/sys/ucontext.h
mm.s: ../include/kernel/kernel.h ../include/sys/ipc.h ../include/signal.h
mm.s: ../include/sys/types.h ../include/sys/syscall.h ../include/sys/unistd.h
mm.s: ../include/stddef.h ../include/util.h ../include/const.h
mm.s: ../include/errno.h ../include/stdbool.h ../include/curses.h
mm.s: ../include/debug.h ../include/string.h ../include/stdio.h
mm.s: ../include/winix/comm.h ../include/winix/compiler.h
mm.s: ../include/winix/type.h ../include/winix/page.h ../include/winix/util.h
mm.s: ../include/winix/kdebug.h ../include/winix/bitmap.h
mm.s: ../include/winix/mm.h ../include/kernel/proc.h ../include/winix/timer.h
mm.s: ../include/winix/kwramp.h ../include/winix/gfp.h
mm.s: ../include/winix/sys_stdio.h ../include/winix/wini_ipc.h
mm.s: ../include/winix/rex.h ../include/winix/mem_alloc.h
mm.s: ../include/kernel/system.h ../include/winix/syscall_ctx.h
mm.s: ../include/ucontext.h ../include/sys/ucontext.h
bitmap.s: ../include/kernel/kernel.h ../include/sys/ipc.h ../include/signal.h
bitmap.s: ../include/sys/types.h ../include/sys/syscall.h
bitmap.s: ../include/sys/unistd.h ../include/stddef.h ../include/util.h
bitmap.s: ../include/const.h ../include/errno.h ../include/stdbool.h
bitmap.s: ../include/curses.h ../include/debug.h ../include/string.h
bitmap.s: ../include/stdio.h ../include/winix/comm.h
bitmap.s: ../include/winix/compiler.h ../include/winix/type.h
bitmap.s: ../include/winix/page.h ../include/winix/util.h
bitmap.s: ../include/winix/kdebug.h ../include/winix/bitmap.h
bitmap.s: ../include/winix/mm.h ../include/kernel/proc.h
bitmap.s: ../include/winix/timer.h ../include/winix/kwramp.h
bitmap.s: ../include/winix/gfp.h ../include/winix/sys_stdio.h
bitmap.s: ../include/winix/wini_ipc.h ../include/winix/rex.h
bitmap.s: ../include/winix/mem_alloc.h ../include/kernel/system.h
bitmap.s: ../include/winix/syscall_ctx.h ../include/ucontext.h
bitmap.s: ../include/sys/ucontext.h
sys_stdio.s: ../include/kernel/kernel.h ../include/sys/ipc.h
sys_stdio.s: ../include/signal.h ../include/sys/types.h
sys_stdio.s: ../include/sys/syscall.h ../include/sys/unistd.h
sys_stdio.s: ../include/stddef.h ../include/util.h ../include/const.h
sys_stdio.s: ../include/errno.h ../include/stdbool.h ../include/curses.h
sys_stdio.s: ../include/debug.h ../include/string.h ../include/stdio.h
sys_stdio.s: ../include/winix/comm.h ../include/winix/compiler.h
sys_stdio.s: ../include/winix/type.h ../include/winix/page.h
sys_stdio.s: ../include/winix/util.h ../include/winix/kdebug.h
sys_stdio.s: ../include/winix/bitmap.h ../include/winix/mm.h
sys_stdio.s: ../include/kernel/proc.h ../include/winix/timer.h
sys_stdio.s: ../include/winix/kwramp.h ../include/winix/gfp.h
sys_stdio.s: ../include/winix/sys_stdio.h ../include/winix/wini_ipc.h
sys_stdio.s: ../include/winix/rex.h ../include/winix/mem_alloc.h
sys_stdio.s: ../include/kernel/system.h ../include/winix/syscall_ctx.h
sys_stdio.s: ../include/ucontext.h ../include/sys/ucontext.h
sys_stdio.s: ../include/ctype.h
wini_ipc.s: ../include/kernel/kernel.h ../include/sys/ipc.h
wini_ipc.s: ../include/signal.h ../include/sys/types.h
wini_ipc.s: ../include/sys/syscall.h ../include/sys/unistd.h
wini_ipc.s: ../include/stddef.h ../include/util.h ../include/const.h
wini_ipc.s: ../include/errno.h ../include/stdbool.h ../include/curses.h
wini_ipc.s: ../include/debug.h ../include/string.h ../include/stdio.h
wini_ipc.s: ../include/winix/comm.h ../include/winix/compiler.h
wini_ipc.s: ../include/winix/type.h ../include/winix/page.h
wini_ipc.s: ../include/winix/util.h ../include/winix/kdebug.h
wini_ipc.s: ../include/winix/bitmap.h ../include/winix/mm.h
wini_ipc.s: ../include/kernel/proc.h ../include/winix/timer.h
wini_ipc.s: ../include/winix/kwramp.h ../include/winix/gfp.h
wini_ipc.s: ../include/winix/sys_stdio.h ../include/winix/wini_ipc.h
wini_ipc.s: ../include/winix/rex.h ../include/winix/mem_alloc.h
wini_ipc.s: ../include/kernel/system.h ../include/winix/syscall_ctx.h
wini_ipc.s: ../include/ucontext.h ../include/sys/ucontext.h
wini_ipc.s: ../include/kernel/exception.h
kdebug.s: ../include/kernel/kernel.h ../include/sys/ipc.h ../include/signal.h
kdebug.s: ../include/sys/types.h ../include/sys/syscall.h
kdebug.s: ../include/sys/unistd.h ../include/stddef.h ../include/util.h
kdebug.s: ../include/const.h ../include/errno.h ../include/stdbool.h
kdebug.s: ../include/curses.h ../include/debug.h ../include/string.h
kdebug.s: ../include/stdio.h ../include/winix/comm.h
kdebug.s: ../include/winix/compiler.h ../include/winix/type.h
kdebug.s: ../include/winix/page.h ../include/winix/util.h
kdebug.s: ../include/winix/kdebug.h ../include/winix/bitmap.h
kdebug.s: ../include/winix/mm.h ../include/kernel/proc.h
kdebug.s: ../include/winix/timer.h ../include/winix/kwramp.h
kdebug.s: ../include/winix/gfp.h ../include/winix/sys_stdio.h
kdebug.s: ../include/winix/wini_ipc.h ../include/winix/rex.h
kdebug.s: ../include/winix/mem_alloc.h ../include/kernel/system.h
kdebug.s: ../include/winix/syscall_ctx.h ../include/ucontext.h
kdebug.s: ../include/sys/ucontext.h
timer.s: ../include/kernel/kernel.h ../include/sys/ipc.h ../include/signal.h
timer.s: ../include/sys/types.h ../include/sys/syscall.h
timer.s: ../include/sys/unistd.h ../include/stddef.h ../include/util.h
timer.s: ../include/const.h ../include/errno.h ../include/stdbool.h
timer.s: ../include/curses.h ../include/debug.h ../include/string.h
timer.s: ../include/stdio.h ../include/winix/comm.h
timer.s: ../include/winix/compiler.h ../include/winix/type.h
timer.s: ../include/winix/page.h ../include/winix/util.h
timer.s: ../include/winix/kdebug.h ../include/winix/bitmap.h
timer.s: ../include/winix/mm.h ../include/kernel/proc.h
timer.s: ../include/winix/timer.h ../include/winix/kwramp.h
timer.s: ../include/winix/gfp.h ../include/winix/sys_stdio.h
timer.s: ../include/winix/wini_ipc.h ../include/winix/rex.h
timer.s: ../include/winix/mem_alloc.h ../include/kernel/system.h
timer.s: ../include/winix/syscall_ctx.h ../include/ucontext.h
timer.s: ../include/sys/ucontext.h ../include/kernel/clock.h
timer.s: ../include/kernel/exception.h
sigsend.s: ../include/kernel/kernel.h ../include/sys/ipc.h
sigsend.s: ../include/signal.h ../include/sys/types.h
sigsend.s: ../include/sys/syscall.h ../include/sys/unistd.h
sigsend.s: ../include/stddef.h ../include/util.h ../include/const.h
sigsend.s: ../include/errno.h ../include/stdbool.h ../include/curses.h
sigsend.s: ../include/debug.h ../include/string.h ../include/stdio.h
sigsend.s: ../include/winix/comm.h ../include/winix/compiler.h
sigsend.s: ../include/winix/type.h ../include/winix/page.h
sigsend.s: ../include/winix/util.h ../include/winix/kdebug.h
sigsend.s: ../include/winix/bitmap.h ../include/winix/mm.h
sigsend.s: ../include/kernel/proc.h ../include/winix/timer.h
sigsend.s: ../include/winix/kwramp.h ../include/winix/gfp.h
sigsend.s: ../include/winix/sys_stdio.h ../include/winix/wini_ipc.h
sigsend.s: ../include/winix/rex.h ../include/winix/mem_alloc.h
sigsend.s: ../include/kernel/system.h ../include/winix/syscall_ctx.h
sigsend.s: ../include/ucontext.h ../include/sys/ucontext.h
sigsend.s: ../include/kernel/exception.h ../include/winix/signal.h
util.s: ../include/kernel/kernel.h ../include/sys/ipc.h ../include/signal.h
util.s: ../include/sys/types.h ../include/sys/syscall.h
util.s: ../include/sys/unistd.h ../include/stddef.h ../include/util.h
util.s: ../include/const.h ../include/errno.h ../include/stdbool.h
util.s: ../include/curses.h ../include/debug.h ../include/string.h
util.s: ../include/stdio.h ../include/winix/comm.h
util.s: ../include/winix/compiler.h ../include/winix/type.h
util.s: ../include/winix/page.h ../include/winix/util.h
util.s: ../include/winix/kdebug.h ../include/winix/bitmap.h
util.s: ../include/winix/mm.h ../include/kernel/proc.h
util.s: ../include/winix/timer.h ../include/winix/kwramp.h
util.s: ../include/winix/gfp.h ../include/winix/sys_stdio.h
util.s: ../include/winix/wini_ipc.h ../include/winix/rex.h
util.s: ../include/winix/mem_alloc.h ../include/kernel/system.h
util.s: ../include/winix/syscall_ctx.h ../include/ucontext.h
util.s: ../include/sys/ucontext.h
