INCLUDE_DIR = ../../include
INCLUDES = $(wildcard $(INCLUDE_DIR)/*.h $(INCLUDE_DIR)/winix/*.h \
				$(INCLUDE_DIR)/sys/*.h $(INCLUDE_DIR)/kernel/*.h)


PROGS = do_alarm do_brk do_execve do_exit do_vfork do_fork \
		do_sigaction do_sigreturn do_wait do_kill do_times\
		do_winfo do_printf do_getc do_getpid do_sysconf
		
SOURCES = $(PROGS:=.c)
ASSEMBLY = $(PROGS:=.s)
OBJECT = $(PROGS:=.o)

.PHONY: depend system

all: | depend system

system: $(OBJECT)
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
	$(Q)makedepend -o.s  -I$(INCLUDE_DIR) $(SOURCES)

clean:
	$(Q)-rm -f $(OBJECT) $(ASSEMBLY)
ifeq ($(KBUILD_VERBOSE),0)
	@echo "RM \t KERNEL/SYSTEM"
endif

.DELETE_ON_ERROR:# DO NOT DELETE

# DO NOT DELETE

do_alarm.s: ../../include/kernel/kernel.h ../../include/sys/ipc.h
do_alarm.s: ../../include/signal.h ../../include/sys/types.h
do_alarm.s: ../../include/sys/syscall.h ../../include/sys/unistd.h
do_alarm.s: ../../include/stddef.h ../../include/util.h ../../include/const.h
do_alarm.s: ../../include/errno.h ../../include/stdbool.h
do_alarm.s: ../../include/curses.h ../../include/debug.h
do_alarm.s: ../../include/string.h ../../include/stdio.h
do_alarm.s: ../../include/winix/comm.h ../../include/winix/compiler.h
do_alarm.s: ../../include/winix/type.h ../../include/winix/page.h
do_alarm.s: ../../include/winix/util.h ../../include/winix/kdebug.h
do_alarm.s: ../../include/winix/bitmap.h ../../include/winix/mm.h
do_alarm.s: ../../include/kernel/proc.h ../../include/winix/timer.h
do_alarm.s: ../../include/winix/kwramp.h ../../include/winix/gfp.h
do_alarm.s: ../../include/winix/sys_stdio.h ../../include/winix/wini_ipc.h
do_alarm.s: ../../include/winix/rex.h ../../include/winix/mem_alloc.h
do_alarm.s: ../../include/kernel/system.h ../../include/winix/syscall_ctx.h
do_alarm.s: ../../include/ucontext.h ../../include/sys/ucontext.h
do_alarm.s: ../../include/kernel/clock.h
do_brk.s: ../../include/kernel/kernel.h ../../include/sys/ipc.h
do_brk.s: ../../include/signal.h ../../include/sys/types.h
do_brk.s: ../../include/sys/syscall.h ../../include/sys/unistd.h
do_brk.s: ../../include/stddef.h ../../include/util.h ../../include/const.h
do_brk.s: ../../include/errno.h ../../include/stdbool.h
do_brk.s: ../../include/curses.h ../../include/debug.h ../../include/string.h
do_brk.s: ../../include/stdio.h ../../include/winix/comm.h
do_brk.s: ../../include/winix/compiler.h ../../include/winix/type.h
do_brk.s: ../../include/winix/page.h ../../include/winix/util.h
do_brk.s: ../../include/winix/kdebug.h ../../include/winix/bitmap.h
do_brk.s: ../../include/winix/mm.h ../../include/kernel/proc.h
do_brk.s: ../../include/winix/timer.h ../../include/winix/kwramp.h
do_brk.s: ../../include/winix/gfp.h ../../include/winix/sys_stdio.h
do_brk.s: ../../include/winix/wini_ipc.h ../../include/winix/rex.h
do_brk.s: ../../include/winix/mem_alloc.h ../../include/kernel/system.h
do_brk.s: ../../include/winix/syscall_ctx.h ../../include/ucontext.h
do_brk.s: ../../include/sys/ucontext.h
do_execve.s: ../../include/kernel/kernel.h ../../include/sys/ipc.h
do_execve.s: ../../include/signal.h ../../include/sys/types.h
do_execve.s: ../../include/sys/syscall.h ../../include/sys/unistd.h
do_execve.s: ../../include/stddef.h ../../include/util.h
do_execve.s: ../../include/const.h ../../include/errno.h
do_execve.s: ../../include/stdbool.h ../../include/curses.h
do_execve.s: ../../include/debug.h ../../include/string.h
do_execve.s: ../../include/stdio.h ../../include/winix/comm.h
do_execve.s: ../../include/winix/compiler.h ../../include/winix/type.h
do_execve.s: ../../include/winix/page.h ../../include/winix/util.h
do_execve.s: ../../include/winix/kdebug.h ../../include/winix/bitmap.h
do_execve.s: ../../include/winix/mm.h ../../include/kernel/proc.h
do_execve.s: ../../include/winix/timer.h ../../include/winix/kwramp.h
do_execve.s: ../../include/winix/gfp.h ../../include/winix/sys_stdio.h
do_execve.s: ../../include/winix/wini_ipc.h ../../include/winix/rex.h
do_execve.s: ../../include/winix/mem_alloc.h ../../include/kernel/system.h
do_execve.s: ../../include/winix/syscall_ctx.h ../../include/ucontext.h
do_execve.s: ../../include/sys/ucontext.h ../../include/winix/srec.h
do_execve.s: ../../include/kernel/table.h
do_exit.s: ../../include/kernel/kernel.h ../../include/sys/ipc.h
do_exit.s: ../../include/signal.h ../../include/sys/types.h
do_exit.s: ../../include/sys/syscall.h ../../include/sys/unistd.h
do_exit.s: ../../include/stddef.h ../../include/util.h ../../include/const.h
do_exit.s: ../../include/errno.h ../../include/stdbool.h
do_exit.s: ../../include/curses.h ../../include/debug.h
do_exit.s: ../../include/string.h ../../include/stdio.h
do_exit.s: ../../include/winix/comm.h ../../include/winix/compiler.h
do_exit.s: ../../include/winix/type.h ../../include/winix/page.h
do_exit.s: ../../include/winix/util.h ../../include/winix/kdebug.h
do_exit.s: ../../include/winix/bitmap.h ../../include/winix/mm.h
do_exit.s: ../../include/kernel/proc.h ../../include/winix/timer.h
do_exit.s: ../../include/winix/kwramp.h ../../include/winix/gfp.h
do_exit.s: ../../include/winix/sys_stdio.h ../../include/winix/wini_ipc.h
do_exit.s: ../../include/winix/rex.h ../../include/winix/mem_alloc.h
do_exit.s: ../../include/kernel/system.h ../../include/winix/syscall_ctx.h
do_exit.s: ../../include/ucontext.h ../../include/sys/ucontext.h
do_vfork.s: ../../include/kernel/kernel.h ../../include/sys/ipc.h
do_vfork.s: ../../include/signal.h ../../include/sys/types.h
do_vfork.s: ../../include/sys/syscall.h ../../include/sys/unistd.h
do_vfork.s: ../../include/stddef.h ../../include/util.h ../../include/const.h
do_vfork.s: ../../include/errno.h ../../include/stdbool.h
do_vfork.s: ../../include/curses.h ../../include/debug.h
do_vfork.s: ../../include/string.h ../../include/stdio.h
do_vfork.s: ../../include/winix/comm.h ../../include/winix/compiler.h
do_vfork.s: ../../include/winix/type.h ../../include/winix/page.h
do_vfork.s: ../../include/winix/util.h ../../include/winix/kdebug.h
do_vfork.s: ../../include/winix/bitmap.h ../../include/winix/mm.h
do_vfork.s: ../../include/kernel/proc.h ../../include/winix/timer.h
do_vfork.s: ../../include/winix/kwramp.h ../../include/winix/gfp.h
do_vfork.s: ../../include/winix/sys_stdio.h ../../include/winix/wini_ipc.h
do_vfork.s: ../../include/winix/rex.h ../../include/winix/mem_alloc.h
do_vfork.s: ../../include/kernel/system.h ../../include/winix/syscall_ctx.h
do_vfork.s: ../../include/ucontext.h ../../include/sys/ucontext.h
do_fork.s: ../../include/kernel/kernel.h ../../include/sys/ipc.h
do_fork.s: ../../include/signal.h ../../include/sys/types.h
do_fork.s: ../../include/sys/syscall.h ../../include/sys/unistd.h
do_fork.s: ../../include/stddef.h ../../include/util.h ../../include/const.h
do_fork.s: ../../include/errno.h ../../include/stdbool.h
do_fork.s: ../../include/curses.h ../../include/debug.h
do_fork.s: ../../include/string.h ../../include/stdio.h
do_fork.s: ../../include/winix/comm.h ../../include/winix/compiler.h
do_fork.s: ../../include/winix/type.h ../../include/winix/page.h
do_fork.s: ../../include/winix/util.h ../../include/winix/kdebug.h
do_fork.s: ../../include/winix/bitmap.h ../../include/winix/mm.h
do_fork.s: ../../include/kernel/proc.h ../../include/winix/timer.h
do_fork.s: ../../include/winix/kwramp.h ../../include/winix/gfp.h
do_fork.s: ../../include/winix/sys_stdio.h ../../include/winix/wini_ipc.h
do_fork.s: ../../include/winix/rex.h ../../include/winix/mem_alloc.h
do_fork.s: ../../include/kernel/system.h ../../include/winix/syscall_ctx.h
do_fork.s: ../../include/ucontext.h ../../include/sys/ucontext.h
do_sigaction.s: ../../include/kernel/kernel.h ../../include/sys/ipc.h
do_sigaction.s: ../../include/signal.h ../../include/sys/types.h
do_sigaction.s: ../../include/sys/syscall.h ../../include/sys/unistd.h
do_sigaction.s: ../../include/stddef.h ../../include/util.h
do_sigaction.s: ../../include/const.h ../../include/errno.h
do_sigaction.s: ../../include/stdbool.h ../../include/curses.h
do_sigaction.s: ../../include/debug.h ../../include/string.h
do_sigaction.s: ../../include/stdio.h ../../include/winix/comm.h
do_sigaction.s: ../../include/winix/compiler.h ../../include/winix/type.h
do_sigaction.s: ../../include/winix/page.h ../../include/winix/util.h
do_sigaction.s: ../../include/winix/kdebug.h ../../include/winix/bitmap.h
do_sigaction.s: ../../include/winix/mm.h ../../include/kernel/proc.h
do_sigaction.s: ../../include/winix/timer.h ../../include/winix/kwramp.h
do_sigaction.s: ../../include/winix/gfp.h ../../include/winix/sys_stdio.h
do_sigaction.s: ../../include/winix/wini_ipc.h ../../include/winix/rex.h
do_sigaction.s: ../../include/winix/mem_alloc.h ../../include/kernel/system.h
do_sigaction.s: ../../include/winix/syscall_ctx.h ../../include/ucontext.h
do_sigaction.s: ../../include/sys/ucontext.h
do_sigreturn.s: ../../include/kernel/kernel.h ../../include/sys/ipc.h
do_sigreturn.s: ../../include/signal.h ../../include/sys/types.h
do_sigreturn.s: ../../include/sys/syscall.h ../../include/sys/unistd.h
do_sigreturn.s: ../../include/stddef.h ../../include/util.h
do_sigreturn.s: ../../include/const.h ../../include/errno.h
do_sigreturn.s: ../../include/stdbool.h ../../include/curses.h
do_sigreturn.s: ../../include/debug.h ../../include/string.h
do_sigreturn.s: ../../include/stdio.h ../../include/winix/comm.h
do_sigreturn.s: ../../include/winix/compiler.h ../../include/winix/type.h
do_sigreturn.s: ../../include/winix/page.h ../../include/winix/util.h
do_sigreturn.s: ../../include/winix/kdebug.h ../../include/winix/bitmap.h
do_sigreturn.s: ../../include/winix/mm.h ../../include/kernel/proc.h
do_sigreturn.s: ../../include/winix/timer.h ../../include/winix/kwramp.h
do_sigreturn.s: ../../include/winix/gfp.h ../../include/winix/sys_stdio.h
do_sigreturn.s: ../../include/winix/wini_ipc.h ../../include/winix/rex.h
do_sigreturn.s: ../../include/winix/mem_alloc.h ../../include/kernel/system.h
do_sigreturn.s: ../../include/winix/syscall_ctx.h ../../include/ucontext.h
do_sigreturn.s: ../../include/sys/ucontext.h ../../include/winix/signal.h
do_wait.s: ../../include/kernel/kernel.h ../../include/sys/ipc.h
do_wait.s: ../../include/signal.h ../../include/sys/types.h
do_wait.s: ../../include/sys/syscall.h ../../include/sys/unistd.h
do_wait.s: ../../include/stddef.h ../../include/util.h ../../include/const.h
do_wait.s: ../../include/errno.h ../../include/stdbool.h
do_wait.s: ../../include/curses.h ../../include/debug.h
do_wait.s: ../../include/string.h ../../include/stdio.h
do_wait.s: ../../include/winix/comm.h ../../include/winix/compiler.h
do_wait.s: ../../include/winix/type.h ../../include/winix/page.h
do_wait.s: ../../include/winix/util.h ../../include/winix/kdebug.h
do_wait.s: ../../include/winix/bitmap.h ../../include/winix/mm.h
do_wait.s: ../../include/kernel/proc.h ../../include/winix/timer.h
do_wait.s: ../../include/winix/kwramp.h ../../include/winix/gfp.h
do_wait.s: ../../include/winix/sys_stdio.h ../../include/winix/wini_ipc.h
do_wait.s: ../../include/winix/rex.h ../../include/winix/mem_alloc.h
do_wait.s: ../../include/kernel/system.h ../../include/winix/syscall_ctx.h
do_wait.s: ../../include/ucontext.h ../../include/sys/ucontext.h
do_wait.s: ../../include/sys/wait.h
do_kill.s: ../../include/kernel/kernel.h ../../include/sys/ipc.h
do_kill.s: ../../include/signal.h ../../include/sys/types.h
do_kill.s: ../../include/sys/syscall.h ../../include/sys/unistd.h
do_kill.s: ../../include/stddef.h ../../include/util.h ../../include/const.h
do_kill.s: ../../include/errno.h ../../include/stdbool.h
do_kill.s: ../../include/curses.h ../../include/debug.h
do_kill.s: ../../include/string.h ../../include/stdio.h
do_kill.s: ../../include/winix/comm.h ../../include/winix/compiler.h
do_kill.s: ../../include/winix/type.h ../../include/winix/page.h
do_kill.s: ../../include/winix/util.h ../../include/winix/kdebug.h
do_kill.s: ../../include/winix/bitmap.h ../../include/winix/mm.h
do_kill.s: ../../include/kernel/proc.h ../../include/winix/timer.h
do_kill.s: ../../include/winix/kwramp.h ../../include/winix/gfp.h
do_kill.s: ../../include/winix/sys_stdio.h ../../include/winix/wini_ipc.h
do_kill.s: ../../include/winix/rex.h ../../include/winix/mem_alloc.h
do_kill.s: ../../include/kernel/system.h ../../include/winix/syscall_ctx.h
do_kill.s: ../../include/ucontext.h ../../include/sys/ucontext.h
do_times.s: ../../include/kernel/kernel.h ../../include/sys/ipc.h
do_times.s: ../../include/signal.h ../../include/sys/types.h
do_times.s: ../../include/sys/syscall.h ../../include/sys/unistd.h
do_times.s: ../../include/stddef.h ../../include/util.h ../../include/const.h
do_times.s: ../../include/errno.h ../../include/stdbool.h
do_times.s: ../../include/curses.h ../../include/debug.h
do_times.s: ../../include/string.h ../../include/stdio.h
do_times.s: ../../include/winix/comm.h ../../include/winix/compiler.h
do_times.s: ../../include/winix/type.h ../../include/winix/page.h
do_times.s: ../../include/winix/util.h ../../include/winix/kdebug.h
do_times.s: ../../include/winix/bitmap.h ../../include/winix/mm.h
do_times.s: ../../include/kernel/proc.h ../../include/winix/timer.h
do_times.s: ../../include/winix/kwramp.h ../../include/winix/gfp.h
do_times.s: ../../include/winix/sys_stdio.h ../../include/winix/wini_ipc.h
do_times.s: ../../include/winix/rex.h ../../include/winix/mem_alloc.h
do_times.s: ../../include/kernel/system.h ../../include/winix/syscall_ctx.h
do_times.s: ../../include/ucontext.h ../../include/sys/ucontext.h
do_times.s: ../../include/sys/times.h
do_winfo.s: ../../include/kernel/kernel.h ../../include/sys/ipc.h
do_winfo.s: ../../include/signal.h ../../include/sys/types.h
do_winfo.s: ../../include/sys/syscall.h ../../include/sys/unistd.h
do_winfo.s: ../../include/stddef.h ../../include/util.h ../../include/const.h
do_winfo.s: ../../include/errno.h ../../include/stdbool.h
do_winfo.s: ../../include/curses.h ../../include/debug.h
do_winfo.s: ../../include/string.h ../../include/stdio.h
do_winfo.s: ../../include/winix/comm.h ../../include/winix/compiler.h
do_winfo.s: ../../include/winix/type.h ../../include/winix/page.h
do_winfo.s: ../../include/winix/util.h ../../include/winix/kdebug.h
do_winfo.s: ../../include/winix/bitmap.h ../../include/winix/mm.h
do_winfo.s: ../../include/kernel/proc.h ../../include/winix/timer.h
do_winfo.s: ../../include/winix/kwramp.h ../../include/winix/gfp.h
do_winfo.s: ../../include/winix/sys_stdio.h ../../include/winix/wini_ipc.h
do_winfo.s: ../../include/winix/rex.h ../../include/winix/mem_alloc.h
do_winfo.s: ../../include/kernel/system.h ../../include/winix/syscall_ctx.h
do_winfo.s: ../../include/ucontext.h ../../include/sys/ucontext.h
do_printf.s: ../../include/kernel/kernel.h ../../include/sys/ipc.h
do_printf.s: ../../include/signal.h ../../include/sys/types.h
do_printf.s: ../../include/sys/syscall.h ../../include/sys/unistd.h
do_printf.s: ../../include/stddef.h ../../include/util.h
do_printf.s: ../../include/const.h ../../include/errno.h
do_printf.s: ../../include/stdbool.h ../../include/curses.h
do_printf.s: ../../include/debug.h ../../include/string.h
do_printf.s: ../../include/stdio.h ../../include/winix/comm.h
do_printf.s: ../../include/winix/compiler.h ../../include/winix/type.h
do_printf.s: ../../include/winix/page.h ../../include/winix/util.h
do_printf.s: ../../include/winix/kdebug.h ../../include/winix/bitmap.h
do_printf.s: ../../include/winix/mm.h ../../include/kernel/proc.h
do_printf.s: ../../include/winix/timer.h ../../include/winix/kwramp.h
do_printf.s: ../../include/winix/gfp.h ../../include/winix/sys_stdio.h
do_printf.s: ../../include/winix/wini_ipc.h ../../include/winix/rex.h
do_printf.s: ../../include/winix/mem_alloc.h ../../include/kernel/system.h
do_printf.s: ../../include/winix/syscall_ctx.h ../../include/ucontext.h
do_printf.s: ../../include/sys/ucontext.h
do_getc.s: ../../include/kernel/kernel.h ../../include/sys/ipc.h
do_getc.s: ../../include/signal.h ../../include/sys/types.h
do_getc.s: ../../include/sys/syscall.h ../../include/sys/unistd.h
do_getc.s: ../../include/stddef.h ../../include/util.h ../../include/const.h
do_getc.s: ../../include/errno.h ../../include/stdbool.h
do_getc.s: ../../include/curses.h ../../include/debug.h
do_getc.s: ../../include/string.h ../../include/stdio.h
do_getc.s: ../../include/winix/comm.h ../../include/winix/compiler.h
do_getc.s: ../../include/winix/type.h ../../include/winix/page.h
do_getc.s: ../../include/winix/util.h ../../include/winix/kdebug.h
do_getc.s: ../../include/winix/bitmap.h ../../include/winix/mm.h
do_getc.s: ../../include/kernel/proc.h ../../include/winix/timer.h
do_getc.s: ../../include/winix/kwramp.h ../../include/winix/gfp.h
do_getc.s: ../../include/winix/sys_stdio.h ../../include/winix/wini_ipc.h
do_getc.s: ../../include/winix/rex.h ../../include/winix/mem_alloc.h
do_getc.s: ../../include/kernel/system.h ../../include/winix/syscall_ctx.h
do_getc.s: ../../include/ucontext.h ../../include/sys/ucontext.h
do_getpid.s: ../../include/kernel/kernel.h ../../include/sys/ipc.h
do_getpid.s: ../../include/signal.h ../../include/sys/types.h
do_getpid.s: ../../include/sys/syscall.h ../../include/sys/unistd.h
do_getpid.s: ../../include/stddef.h ../../include/util.h
do_getpid.s: ../../include/const.h ../../include/errno.h
do_getpid.s: ../../include/stdbool.h ../../include/curses.h
do_getpid.s: ../../include/debug.h ../../include/string.h
do_getpid.s: ../../include/stdio.h ../../include/winix/comm.h
do_getpid.s: ../../include/winix/compiler.h ../../include/winix/type.h
do_getpid.s: ../../include/winix/page.h ../../include/winix/util.h
do_getpid.s: ../../include/winix/kdebug.h ../../include/winix/bitmap.h
do_getpid.s: ../../include/winix/mm.h ../../include/kernel/proc.h
do_getpid.s: ../../include/winix/timer.h ../../include/winix/kwramp.h
do_getpid.s: ../../include/winix/gfp.h ../../include/winix/sys_stdio.h
do_getpid.s: ../../include/winix/wini_ipc.h ../../include/winix/rex.h
do_getpid.s: ../../include/winix/mem_alloc.h ../../include/kernel/system.h
do_getpid.s: ../../include/winix/syscall_ctx.h ../../include/ucontext.h
do_getpid.s: ../../include/sys/ucontext.h
do_sysconf.s: ../../include/kernel/kernel.h ../../include/sys/ipc.h
do_sysconf.s: ../../include/signal.h ../../include/sys/types.h
do_sysconf.s: ../../include/sys/syscall.h ../../include/sys/unistd.h
do_sysconf.s: ../../include/stddef.h ../../include/util.h
do_sysconf.s: ../../include/const.h ../../include/errno.h
do_sysconf.s: ../../include/stdbool.h ../../include/curses.h
do_sysconf.s: ../../include/debug.h ../../include/string.h
do_sysconf.s: ../../include/stdio.h ../../include/winix/comm.h
do_sysconf.s: ../../include/winix/compiler.h ../../include/winix/type.h
do_sysconf.s: ../../include/winix/page.h ../../include/winix/util.h
do_sysconf.s: ../../include/winix/kdebug.h ../../include/winix/bitmap.h
do_sysconf.s: ../../include/winix/mm.h ../../include/kernel/proc.h
do_sysconf.s: ../../include/winix/timer.h ../../include/winix/kwramp.h
do_sysconf.s: ../../include/winix/gfp.h ../../include/winix/sys_stdio.h
do_sysconf.s: ../../include/winix/wini_ipc.h ../../include/winix/rex.h
do_sysconf.s: ../../include/winix/mem_alloc.h ../../include/kernel/system.h
do_sysconf.s: ../../include/winix/syscall_ctx.h ../../include/ucontext.h
do_sysconf.s: ../../include/sys/ucontext.h
