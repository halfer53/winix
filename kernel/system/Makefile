WINIXO = winix/*.o
sources = sys_alarm.c sys_brk.c sys_exec.c sys_exit.c sys_fork.c sys_sbrk.c sys_sigaction.c sys_sigreturn.c sys_wait.c sys_sigsend.c
compile = sys_alarm.s sys_brk.s sys_exec.s sys_exit.s sys_fork.s sys_sbrk.s sys_sigaction.s sys_sigreturn.s sys_wait.s sys_sigsend.s
assemble = sys_alarm.o sys_brk.o sys_exec.o sys_exit.o sys_fork.o sys_sbrk.o sys_sigaction.o sys_sigreturn.o sys_wait.o sys_sigsend.o

all: $(assemble)
$(compile): %.s: %.c
	wcc -S -I../../include -o $@ $<

$(assemble): %.o: %.s
	wasm $< -o $@

$(link): $(assemble)
	wlink -o $(link) $(assemble)

depend:
	makedepend $(sources)

clean:
	-rm -f $(compile) $(assemble)

.DELETE_ON_ERROR: