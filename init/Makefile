MAIN_DIR = ..
INCLUDE_DIR = $(MAIN_DIR)/include
INCLUDES = $(wildcard $(INCLUDE_DIR)/*.h $(INCLUDE_DIR)/winix/*.h $(INCLUDE_DIR)/sys/*.h $(INCLUDE_DIR)/kernel/*.h)

PROGS = init
SOURCES = $(PROGS:=.c)
ASSEMBLY = $(PROGS:=.s)
OBJECT = $(PROGS:=.o)
OUT = init.srec

LIBS = syscall/*.o stdlib/errno.o posix/_sigset.o
LIBS_O = $(addprefix ../lib/, $(LIBS))

.PHONY: depend wramp init 

init: | depend wramp init.srec

init.srec: $(OBJECT) $(LIBS_O)
	$(Q)wlink -v -o $(OUT) init.o $(LIBS_O) > $(MAIN_DIR)/tools/kdbg_srec/init.kdbg
	$(Q)java -cp $(MAIN_DIR)/tools reformat_srec init.srec
	$(Q)./$(MAIN_DIR)/tools/gen_bin_code init.srec > $(INCLUDE_DIR)/init_bin.c

ifeq ($(KBUILD_VERBOSE),0)
	@echo "LD \t INIT\nBIN\t INIT"
endif

.PHONY: depend init

all: | depend init

init: $(OBJECT)

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
	$(Q)-rm -f $(OBJECT) $(ASSEMBLY) $(OUT)
ifeq ($(KBUILD_VERBOSE),0)
	@echo "RM \t INIT"
endif

.DELETE_ON_ERROR:
include .depend
