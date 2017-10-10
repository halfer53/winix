
INCLUDE_DIR = ../include
INCLUDES = $(wildcard $(INCLUDE_DIR)/*.h $(INCLUDE_DIR)/winix/*.h\
				 $(INCLUDE_DIR)/sys/*.h $(INCLUDE_DIR)/kernel/*.h)

PROGS = main proc system exception idle clock sched table
SOURCES = $(PROGS:=.c)
ASSEMBLY = $(PROGS:=.s)
OBJECT = $(PROGS:=.o)

.PHONY: kernel depend

all: | depend kernel

kernel:$(OBJECT) $(LIMITS)
	$(Q)$(MAKE) -C system

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

depend:.depend

.depend:$(SOURCES)
	$(Q)$(MAKE) -C system depend
	$(Q)makedepend -o.s -I$(INCLUDE_DIR) $(SOURCES)

clean:
	$(Q)$(MAKE) -C system clean
	$(Q)-rm -f $(OBJECT) $(ASSEMBLY) $(LIMITS)
ifeq ($(KBUILD_VERBOSE),0)
	@echo "RM \t KERNEL"
endif

.DELETE_ON_ERROR:# DO NOT DELETE
include .depend# DO NOT DELETE

main.s: ../include/kernel/kernel.h ../include/sys/ipc.h ../include/signal.h
main.s: ../include/sys/types.h ../include/sys/syscall.h
main.s: ../include/sys/unistd.h ../include/stddef.h ../include/util.h
main.s: ../include/const.h ../include/errno.h ../include/stdbool.h
main.s: ../include/curses.h ../include/debug.h ../include/string.h
main.s: ../include/stdio.h ../include/winix/comm.h
main.s: ../include/winix/compiler.h ../include/winix/type.h
main.s: ../include/winix/page.h ../include/winix/util.h
main.s: ../include/winix/kdebug.h ../include/winix/bitmap.h
main.s: ../include/winix/mm.h ../include/kernel/proc.h
main.s: ../include/winix/timer.h ../include/winix/kwramp.h
main.s: ../include/winix/gfp.h ../include/winix/sys_stdio.h
main.s: ../include/winix/wini_ipc.h ../include/winix/rex.h
main.s: ../include/winix/mem_alloc.h ../include/kernel/system.h
main.s: ../include/winix/syscall_ctx.h ../include/ucontext.h
main.s: ../include/sys/ucontext.h ../include/kernel/table.h
main.s: ../include/kernel/clock.h ../include/kernel/idle.h
main.s: ../include/init_bin.c ../include/shell_bin.c
proc.s: ../include/kernel/kernel.h ../include/sys/ipc.h ../include/signal.h
proc.s: ../include/sys/types.h ../include/sys/syscall.h
proc.s: ../include/sys/unistd.h ../include/stddef.h ../include/util.h
proc.s: ../include/const.h ../include/errno.h ../include/stdbool.h
proc.s: ../include/curses.h ../include/debug.h ../include/string.h
proc.s: ../include/stdio.h ../include/winix/comm.h
proc.s: ../include/winix/compiler.h ../include/winix/type.h
proc.s: ../include/winix/page.h ../include/winix/util.h
proc.s: ../include/winix/kdebug.h ../include/winix/bitmap.h
proc.s: ../include/winix/mm.h ../include/kernel/proc.h
proc.s: ../include/winix/timer.h ../include/winix/kwramp.h
proc.s: ../include/winix/gfp.h ../include/winix/sys_stdio.h
proc.s: ../include/winix/wini_ipc.h ../include/winix/rex.h
proc.s: ../include/winix/mem_alloc.h ../include/kernel/system.h
proc.s: ../include/winix/syscall_ctx.h ../include/ucontext.h
proc.s: ../include/sys/ucontext.h ../include/winix/srec.h
system.s: ../include/kernel/kernel.h ../include/sys/ipc.h ../include/signal.h
system.s: ../include/sys/types.h ../include/sys/syscall.h
system.s: ../include/sys/unistd.h ../include/stddef.h ../include/util.h
system.s: ../include/const.h ../include/errno.h ../include/stdbool.h
system.s: ../include/curses.h ../include/debug.h ../include/string.h
system.s: ../include/stdio.h ../include/winix/comm.h
system.s: ../include/winix/compiler.h ../include/winix/type.h
system.s: ../include/winix/page.h ../include/winix/util.h
system.s: ../include/winix/kdebug.h ../include/winix/bitmap.h
system.s: ../include/winix/mm.h ../include/kernel/proc.h
system.s: ../include/winix/timer.h ../include/winix/kwramp.h
system.s: ../include/winix/gfp.h ../include/winix/sys_stdio.h
system.s: ../include/winix/wini_ipc.h ../include/winix/rex.h
system.s: ../include/winix/mem_alloc.h ../include/kernel/system.h
system.s: ../include/winix/syscall_ctx.h ../include/ucontext.h
system.s: ../include/sys/ucontext.h ../include/kernel/sched.h
system.s: ../include/kernel/table.h ../include/kernel/clock.h
system.s: ../include/kernel/idle.h
exception.s: ../include/kernel/kernel.h ../include/sys/ipc.h
exception.s: ../include/signal.h ../include/sys/types.h
exception.s: ../include/sys/syscall.h ../include/sys/unistd.h
exception.s: ../include/stddef.h ../include/util.h ../include/const.h
exception.s: ../include/errno.h ../include/stdbool.h ../include/curses.h
exception.s: ../include/debug.h ../include/string.h ../include/stdio.h
exception.s: ../include/winix/comm.h ../include/winix/compiler.h
exception.s: ../include/winix/type.h ../include/winix/page.h
exception.s: ../include/winix/util.h ../include/winix/kdebug.h
exception.s: ../include/winix/bitmap.h ../include/winix/mm.h
exception.s: ../include/kernel/proc.h ../include/winix/timer.h
exception.s: ../include/winix/kwramp.h ../include/winix/gfp.h
exception.s: ../include/winix/sys_stdio.h ../include/winix/wini_ipc.h
exception.s: ../include/winix/rex.h ../include/winix/mem_alloc.h
exception.s: ../include/kernel/system.h ../include/winix/syscall_ctx.h
exception.s: ../include/ucontext.h ../include/sys/ucontext.h
exception.s: ../include/kernel/exception.h ../include/kernel/clock.h
idle.s: ../include/winix/rex.h ../include/kernel/idle.h
clock.s: ../include/kernel/kernel.h ../include/sys/ipc.h ../include/signal.h
clock.s: ../include/sys/types.h ../include/sys/syscall.h
clock.s: ../include/sys/unistd.h ../include/stddef.h ../include/util.h
clock.s: ../include/const.h ../include/errno.h ../include/stdbool.h
clock.s: ../include/curses.h ../include/debug.h ../include/string.h
clock.s: ../include/stdio.h ../include/winix/comm.h
clock.s: ../include/winix/compiler.h ../include/winix/type.h
clock.s: ../include/winix/page.h ../include/winix/util.h
clock.s: ../include/winix/kdebug.h ../include/winix/bitmap.h
clock.s: ../include/winix/mm.h ../include/kernel/proc.h
clock.s: ../include/winix/timer.h ../include/winix/kwramp.h
clock.s: ../include/winix/gfp.h ../include/winix/sys_stdio.h
clock.s: ../include/winix/wini_ipc.h ../include/winix/rex.h
clock.s: ../include/winix/mem_alloc.h ../include/kernel/system.h
clock.s: ../include/winix/syscall_ctx.h ../include/ucontext.h
clock.s: ../include/sys/ucontext.h ../include/kernel/clock.h
clock.s: ../include/kernel/table.h ../include/kernel/idle.h
clock.s: ../include/kernel/exception.h
sched.s: ../include/kernel/kernel.h ../include/sys/ipc.h ../include/signal.h
sched.s: ../include/sys/types.h ../include/sys/syscall.h
sched.s: ../include/sys/unistd.h ../include/stddef.h ../include/util.h
sched.s: ../include/const.h ../include/errno.h ../include/stdbool.h
sched.s: ../include/curses.h ../include/debug.h ../include/string.h
sched.s: ../include/stdio.h ../include/winix/comm.h
sched.s: ../include/winix/compiler.h ../include/winix/type.h
sched.s: ../include/winix/page.h ../include/winix/util.h
sched.s: ../include/winix/kdebug.h ../include/winix/bitmap.h
sched.s: ../include/winix/mm.h ../include/kernel/proc.h
sched.s: ../include/winix/timer.h ../include/winix/kwramp.h
sched.s: ../include/winix/gfp.h ../include/winix/sys_stdio.h
sched.s: ../include/winix/wini_ipc.h ../include/winix/rex.h
sched.s: ../include/winix/mem_alloc.h ../include/kernel/system.h
sched.s: ../include/winix/syscall_ctx.h ../include/ucontext.h
sched.s: ../include/sys/ucontext.h ../include/kernel/exception.h
sched.s: ../include/kernel/sched.h
table.s: ../include/kernel/kernel.h ../include/sys/ipc.h ../include/signal.h
table.s: ../include/sys/types.h ../include/sys/syscall.h
table.s: ../include/sys/unistd.h ../include/stddef.h ../include/util.h
table.s: ../include/const.h ../include/errno.h ../include/stdbool.h
table.s: ../include/curses.h ../include/debug.h ../include/string.h
table.s: ../include/stdio.h ../include/winix/comm.h
table.s: ../include/winix/compiler.h ../include/winix/type.h
table.s: ../include/winix/page.h ../include/winix/util.h
table.s: ../include/winix/kdebug.h ../include/winix/bitmap.h
table.s: ../include/winix/mm.h ../include/kernel/proc.h
table.s: ../include/winix/timer.h ../include/winix/kwramp.h
table.s: ../include/winix/gfp.h ../include/winix/sys_stdio.h
table.s: ../include/winix/wini_ipc.h ../include/winix/rex.h
table.s: ../include/winix/mem_alloc.h ../include/kernel/system.h
table.s: ../include/winix/syscall_ctx.h ../include/ucontext.h
table.s: ../include/sys/ucontext.h ../include/kernel/table.h
table.s: ../include/kernel/clock.h ../include/kernel/idle.h
# DO NOT DELETE

main.s: ../include/kernel/kernel.h ../include/sys/ipc.h ../include/signal.h
main.s: ../include/sys/types.h ../include/sys/syscall.h
main.s: ../include/sys/unistd.h ../include/stddef.h ../include/util.h
main.s: ../include/const.h ../include/errno.h ../include/stdbool.h
main.s: ../include/curses.h ../include/debug.h ../include/string.h
main.s: ../include/stdio.h ../include/winix/comm.h
main.s: ../include/winix/compiler.h ../include/winix/type.h
main.s: ../include/winix/page.h ../include/winix/util.h
main.s: ../include/winix/kdebug.h ../include/winix/bitmap.h
main.s: ../include/winix/mm.h ../include/kernel/proc.h
main.s: ../include/winix/timer.h ../include/winix/kwramp.h
main.s: ../include/winix/gfp.h ../include/winix/sys_stdio.h
main.s: ../include/winix/wini_ipc.h ../include/winix/rex.h
main.s: ../include/winix/mem_alloc.h ../include/kernel/system.h
main.s: ../include/winix/syscall_ctx.h ../include/ucontext.h
main.s: ../include/sys/ucontext.h ../include/kernel/table.h
main.s: ../include/kernel/clock.h ../include/kernel/idle.h
main.s: ../include/init_bin.c ../include/shell_bin.c
proc.s: ../include/kernel/kernel.h ../include/sys/ipc.h ../include/signal.h
proc.s: ../include/sys/types.h ../include/sys/syscall.h
proc.s: ../include/sys/unistd.h ../include/stddef.h ../include/util.h
proc.s: ../include/const.h ../include/errno.h ../include/stdbool.h
proc.s: ../include/curses.h ../include/debug.h ../include/string.h
proc.s: ../include/stdio.h ../include/winix/comm.h
proc.s: ../include/winix/compiler.h ../include/winix/type.h
proc.s: ../include/winix/page.h ../include/winix/util.h
proc.s: ../include/winix/kdebug.h ../include/winix/bitmap.h
proc.s: ../include/winix/mm.h ../include/kernel/proc.h
proc.s: ../include/winix/timer.h ../include/winix/kwramp.h
proc.s: ../include/winix/gfp.h ../include/winix/sys_stdio.h
proc.s: ../include/winix/wini_ipc.h ../include/winix/rex.h
proc.s: ../include/winix/mem_alloc.h ../include/kernel/system.h
proc.s: ../include/winix/syscall_ctx.h ../include/ucontext.h
proc.s: ../include/sys/ucontext.h ../include/winix/srec.h
system.s: ../include/kernel/kernel.h ../include/sys/ipc.h ../include/signal.h
system.s: ../include/sys/types.h ../include/sys/syscall.h
system.s: ../include/sys/unistd.h ../include/stddef.h ../include/util.h
system.s: ../include/const.h ../include/errno.h ../include/stdbool.h
system.s: ../include/curses.h ../include/debug.h ../include/string.h
system.s: ../include/stdio.h ../include/winix/comm.h
system.s: ../include/winix/compiler.h ../include/winix/type.h
system.s: ../include/winix/page.h ../include/winix/util.h
system.s: ../include/winix/kdebug.h ../include/winix/bitmap.h
system.s: ../include/winix/mm.h ../include/kernel/proc.h
system.s: ../include/winix/timer.h ../include/winix/kwramp.h
system.s: ../include/winix/gfp.h ../include/winix/sys_stdio.h
system.s: ../include/winix/wini_ipc.h ../include/winix/rex.h
system.s: ../include/winix/mem_alloc.h ../include/kernel/system.h
system.s: ../include/winix/syscall_ctx.h ../include/ucontext.h
system.s: ../include/sys/ucontext.h ../include/kernel/sched.h
system.s: ../include/kernel/table.h ../include/kernel/clock.h
system.s: ../include/kernel/idle.h
exception.s: ../include/kernel/kernel.h ../include/sys/ipc.h
exception.s: ../include/signal.h ../include/sys/types.h
exception.s: ../include/sys/syscall.h ../include/sys/unistd.h
exception.s: ../include/stddef.h ../include/util.h ../include/const.h
exception.s: ../include/errno.h ../include/stdbool.h ../include/curses.h
exception.s: ../include/debug.h ../include/string.h ../include/stdio.h
exception.s: ../include/winix/comm.h ../include/winix/compiler.h
exception.s: ../include/winix/type.h ../include/winix/page.h
exception.s: ../include/winix/util.h ../include/winix/kdebug.h
exception.s: ../include/winix/bitmap.h ../include/winix/mm.h
exception.s: ../include/kernel/proc.h ../include/winix/timer.h
exception.s: ../include/winix/kwramp.h ../include/winix/gfp.h
exception.s: ../include/winix/sys_stdio.h ../include/winix/wini_ipc.h
exception.s: ../include/winix/rex.h ../include/winix/mem_alloc.h
exception.s: ../include/kernel/system.h ../include/winix/syscall_ctx.h
exception.s: ../include/ucontext.h ../include/sys/ucontext.h
exception.s: ../include/kernel/exception.h ../include/kernel/clock.h
idle.s: ../include/winix/rex.h ../include/kernel/idle.h
clock.s: ../include/kernel/kernel.h ../include/sys/ipc.h ../include/signal.h
clock.s: ../include/sys/types.h ../include/sys/syscall.h
clock.s: ../include/sys/unistd.h ../include/stddef.h ../include/util.h
clock.s: ../include/const.h ../include/errno.h ../include/stdbool.h
clock.s: ../include/curses.h ../include/debug.h ../include/string.h
clock.s: ../include/stdio.h ../include/winix/comm.h
clock.s: ../include/winix/compiler.h ../include/winix/type.h
clock.s: ../include/winix/page.h ../include/winix/util.h
clock.s: ../include/winix/kdebug.h ../include/winix/bitmap.h
clock.s: ../include/winix/mm.h ../include/kernel/proc.h
clock.s: ../include/winix/timer.h ../include/winix/kwramp.h
clock.s: ../include/winix/gfp.h ../include/winix/sys_stdio.h
clock.s: ../include/winix/wini_ipc.h ../include/winix/rex.h
clock.s: ../include/winix/mem_alloc.h ../include/kernel/system.h
clock.s: ../include/winix/syscall_ctx.h ../include/ucontext.h
clock.s: ../include/sys/ucontext.h ../include/kernel/clock.h
clock.s: ../include/kernel/table.h ../include/kernel/idle.h
clock.s: ../include/kernel/exception.h
sched.s: ../include/kernel/kernel.h ../include/sys/ipc.h ../include/signal.h
sched.s: ../include/sys/types.h ../include/sys/syscall.h
sched.s: ../include/sys/unistd.h ../include/stddef.h ../include/util.h
sched.s: ../include/const.h ../include/errno.h ../include/stdbool.h
sched.s: ../include/curses.h ../include/debug.h ../include/string.h
sched.s: ../include/stdio.h ../include/winix/comm.h
sched.s: ../include/winix/compiler.h ../include/winix/type.h
sched.s: ../include/winix/page.h ../include/winix/util.h
sched.s: ../include/winix/kdebug.h ../include/winix/bitmap.h
sched.s: ../include/winix/mm.h ../include/kernel/proc.h
sched.s: ../include/winix/timer.h ../include/winix/kwramp.h
sched.s: ../include/winix/gfp.h ../include/winix/sys_stdio.h
sched.s: ../include/winix/wini_ipc.h ../include/winix/rex.h
sched.s: ../include/winix/mem_alloc.h ../include/kernel/system.h
sched.s: ../include/winix/syscall_ctx.h ../include/ucontext.h
sched.s: ../include/sys/ucontext.h ../include/kernel/exception.h
sched.s: ../include/kernel/sched.h
table.s: ../include/kernel/kernel.h ../include/sys/ipc.h ../include/signal.h
table.s: ../include/sys/types.h ../include/sys/syscall.h
table.s: ../include/sys/unistd.h ../include/stddef.h ../include/util.h
table.s: ../include/const.h ../include/errno.h ../include/stdbool.h
table.s: ../include/curses.h ../include/debug.h ../include/string.h
table.s: ../include/stdio.h ../include/winix/comm.h
table.s: ../include/winix/compiler.h ../include/winix/type.h
table.s: ../include/winix/page.h ../include/winix/util.h
table.s: ../include/winix/kdebug.h ../include/winix/bitmap.h
table.s: ../include/winix/mm.h ../include/kernel/proc.h
table.s: ../include/winix/timer.h ../include/winix/kwramp.h
table.s: ../include/winix/gfp.h ../include/winix/sys_stdio.h
table.s: ../include/winix/wini_ipc.h ../include/winix/rex.h
table.s: ../include/winix/mem_alloc.h ../include/kernel/system.h
table.s: ../include/winix/syscall_ctx.h ../include/ucontext.h
table.s: ../include/sys/ucontext.h ../include/kernel/table.h
table.s: ../include/kernel/clock.h ../include/kernel/idle.h
