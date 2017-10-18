
INCLUDE_DIR = ../include

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
	$(Q)makedepend -o.s -f- -I$(INCLUDE_DIR) $(SOURCES) > .depend

clean:
	$(Q)echo -n "" > .depend
	$(Q)$(MAKE) -C system clean
	$(Q)-rm -f $(OBJECT) $(ASSEMBLY) $(LIMITS)
ifeq ($(KBUILD_VERBOSE),0)
	@echo "RM \t KERNEL"
endif

.DELETE_ON_ERROR:# DO NOT DELETE
include .depend# DO NOT DELETE