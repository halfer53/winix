MAIN_DIR = ..
INCLUDE_DIR = $(MAIN_DIR)/include
INCLUDES = $(wildcard $(INCLUDE_DIR)/*.h $(INCLUDE_DIR)/winix/*.h $(INCLUDE_DIR)/sys/*.h $(INCLUDE_DIR)/kernel/*.h)

PROGS = init
SOURCES = $(PROGS:=.c)
ASSEMBLY = $(PROGS:=.s)
OBJECT = $(PROGS:=.o)
OUT = init.srec

LIBS = syscall/syscall.o syscall/wramp_syscall.o syscall/ipc_receive.o\
		 syscall/ipc_send.o stdlib/errno.o syscall/wait.o syscall/printf.o\
		  syscall/_exit.o 
LIBS_O = $(addprefix ../lib/, $(LIBS))

.PHONY: wramp init 

init: | wramp init.srec

init.srec: $(OBJECT) $(LIBS_O)
	$(Q)wlink -o $(OUT) init.o $(LIBS_O)
	$(Q)java -cp $(MAIN_DIR)/tools reformat_srec init.srec
	$(Q)./$(MAIN_DIR)/tools/gen_bin_code init.srec > $(INCLUDE_DIR)/init_bin.c

ifeq ($(KBUILD_VERBOSE),0)
	@echo "LD \t INIT\nBIN\t INIT"
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
