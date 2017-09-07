INCLUDE_DIR = ../include
INCLUDES = $(wildcard $(INCLUDE_DIR)/*.h $(INCLUDE_DIR)/winix/*.h $(INCLUDE_DIR)/sys/*.h $(INCLUDE_DIR)/kernel/*.h)

PROGS = init
SOURCES = $(PROGS:=.c)
ASSEMBLY = $(PROGS:=.s)
OBJECT = $(PROGS:=.o)
OUT = init.srec

LIBS = syscall/syscall.o syscall/wramp_syscall.o syscall/ipc.o stdlib/errno.o\
		syscall/wait.o syscall/printf.o syscall/_exit.o 
LIBS_O = $(addprefix ../lib/, $(LIBS))

init: wramp
	$(Q)wlink -o init.srec init.o $(LIBS_O)
ifeq ($(KBUILD_VERBOSE),0)
	@echo "LD \t INIT"
endif

wramp: $(OBJECT)

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
	$(Q)-rm -f $(OBJECT) $(ASSEMBLY) $(OUT)
ifeq ($(KBUILD_VERBOSE),0)
	@echo "RM \t INIT"
endif

.DELETE_ON_ERROR:
