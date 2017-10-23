INCLUDE_DIR = ../../include
INCLUDES = $(wildcard $(INCLUDE_DIR)/*.h $(INCLUDE_DIR)/winix/*.h \
				$(INCLUDE_DIR)/sys/*.h $(INCLUDE_DIR)/kernel/*.h)


PROGS = do_alarm do_brk do_execve do_exit_wait do_vfork do_fork \
		do_sigaction do_sigreturn do_kill do_times\
		do_winfo do_printf do_getc do_getpid do_sysconf\
		do_sigpending do_sigprocmask do_sigsuspend do_setpgid\
		do_getpgid
		
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

depend:.depend

.depend:$(SOURCES)
	$(Q)makedepend -o.s -f- -I$(INCLUDE_DIR) $(SOURCES) > .depend

clean:
	$(Q)echo -n "" > .depend
	$(Q)-rm -f $(OBJECT) $(ASSEMBLY)
ifeq ($(KBUILD_VERBOSE),0)
	@echo "RM \t KERNEL/SYSTEM"
endif

.DELETE_ON_ERROR:# DO NOT DELETE
include .depend
