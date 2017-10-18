INCLUDE_DIR = ../include
INCLUDES = $(wildcard $(INCLUDE_DIR)/*.h $(INCLUDE_DIR)/sys/*.h)

MODULES := ansi gen posix stdio stdlib syscall util
SOURCES = $(foreach sdir, $(MODULES), $(wildcard $(sdir)/*.c))
ASSEMBLY = $(SOURCES:.c=.s)
OBJECT = $(SOURCES:.c=.o) syscall/wramp_syscall.o util/debug.o gen/ucontext.o\
							syscall/vfork.o	syscall/__sigreturn.o

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

depend:.depend

.depend:$(SOURCES)
	$(Q)makedepend -o.s -f- -I$(INCLUDE_DIR) $(SOURCES) > .depend

clean:
	$(Q)echo -n "" > .depend
	$(Q)-rm -f $(OBJECT) $(ASSEMBLY)
ifeq ($(KBUILD_VERBOSE),0)
	@echo "RM \t LIB"
endif

.DELETE_ON_ERROR:
# DO NOT DELETE
include .depend