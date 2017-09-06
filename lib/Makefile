INCLUDE_DIR = ../include
INCLUDES = $(wildcard $(INCLUDE_DIR)/*.h $(INCLUDE_DIR)/sys/*.h)

MODULES := ansi gen posix stdio stdlib syscall util
SOURCES = $(foreach sdir, $(MODULES), $(wildcard $(sdir)/*.c))
ASSEMBLY = $(SOURCES:.c=.s)
OBJECT = $(SOURCES:.c=.o) syscall/wramp_syscall.o util/debug.o gen/ucontext.o



wramp: $(OBJECT)
$(ASSEMBLY): %.s: %.c $(INCLUDES)
	@wcc $(CFLAGS) -S -N -I$(INCLUDE_DIR) -o $@ $<
	@echo "CC \t $<"
	
$(OBJECT): %.o: %.s
	@wasm $< -o $@
	@echo "AS \t $<"

depend:
	makedepend -I$(INCLUDE_DIR) $(SOURCES)

clean:
	@-rm -f $(OBJECT) $(ASSEMBLY)
	@echo "RM \t LIB"

.DELETE_ON_ERROR:
