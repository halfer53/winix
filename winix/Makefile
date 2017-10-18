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

depend: .depend

.depend:$(SOURCES)
	$(Q)makedepend -o.s -f- -I$(INCLUDE_DIR) $(SOURCES) > .depend

clean:
	$(Q)echo -n "" > .depend
	$(Q)-rm -f $(OBJECT) $(ASSEMBLY)
ifeq ($(KBUILD_VERBOSE),0)
	@echo "RM \t WINIX"
endif

.DELETE_ON_ERROR:
# DO NOT DELETE
include .depend