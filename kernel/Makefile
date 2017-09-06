
INCLUDE_DIR = ../include
INCLUDES = $(wildcard $(INCLUDE_DIR)/*.h $(INCLUDE_DIR)/winix/*.h\
				 $(INCLUDE_DIR)/sys/*.h $(INCLUDE_DIR)/kernel/*.h)

PROGS = main proc system exception idle clock sched
SOURCES = $(PROGS:=.c)
ASSEMBLY = $(PROGS:=.s)
OBJECT = $(PROGS:=.o)

kernel:$(OBJECT) $(INCLUDES) $(LIMITS)
	$(Q)$(MAKE) -C system

$(ASSEMBLY): %.s: %.c $(INCLUDES)
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
	$(MAKE) -C system depend
	makedepend -I$(INCLUDE_DIR) $(SOURCES)

clean:
	$(Q)$(MAKE) -C system clean
	$(Q)-rm -f $(OBJECT) $(ASSEMBLY) $(LIMITS)
ifeq ($(KBUILD_VERBOSE),0)
	@echo "RM \t KERNEL"
endif

.DELETE_ON_ERROR:# DO NOT DELETE


