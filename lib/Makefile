INCLUDE_DIR = ../include
INCLUDES = $(wildcard $(INCLUDE_DIR)/*.h $(INCLUDE_DIR)/sys/*.h)

MODULES := ansi gen posix stdio stdlib syscall util
SOURCES = $(foreach sdir, $(MODULES), $(wildcard $(sdir)/*.c))
ASSEMBLY = $(SOURCES:.c=.s)
OBJECT = $(SOURCES:.c=.o) syscall/wramp_syscall.o util/debug.o gen/ucontext.o



lib: $(OBJECT)
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
	makedepend -I$(INCLUDE_DIR) $(SOURCES)

clean:
	$(Q)-rm -f $(OBJECT) $(ASSEMBLY)
ifeq ($(KBUILD_VERBOSE),0)
	@echo "RM \t LIB"
endif

.DELETE_ON_ERROR:
