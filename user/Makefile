MAIN_DIR = ..
INCLUDE_DIR = $(MAIN_DIR)/include
INCLUDES = $(wildcard $(INCLUDE_DIR)/*.h $(INCLUDE_DIR)/sys/*.h )
LIBS_O = $(shell find ../lib -name "*.o")

SOURCES = 
ASSEMBLY = 
OBJECT = 

SHELL_PROGS = shell shell_parse shell_test
SHELL_SOURCES = $(SHELL_PROGS:=.c)
SHELL_ASSEMBLY = $(SHELL_PROGS:=.s)
SHELL_OBJECT = $(SHELL_PROGS:=.o)
SHELL_SREC = shell.srec

SOURCES += $(SHELL_SOURCES)
ASSEMBLY += $(SHELL_ASSEMBLY)
OBJECT += $(SHELL_OBJECT)

.PHONY : user shell

shell: | user shell.srec	


shell.srec: $(OBJECT) $(LIBS_O) 
	$(Q)wlink -o $(SHELL_SREC) $(OBJECT) $(LIBS_O)
	$(Q)java -cp $(MAIN_DIR)/tools reformat_srec $(SHELL_SREC)
	$(Q)./$(MAIN_DIR)/tools/gen_bin_code $(SHELL_SREC) > $(INCLUDE_DIR)/shell_bin.c

ifeq ($(KBUILD_VERBOSE),0)
	@echo "LD \t SHELL\nBIN\t SHELL"
endif

user: $(OBJECT)
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
	makedepend -I$(INCLUDE_DIR) $(SOURCES) $(SHELL_SREC)

clean:
	$(Q)-rm -f $(OBJECT) $(ASSEMBLY) $(SHELL_SREC)
ifeq ($(KBUILD_VERBOSE),0)
	@echo "RM \t USER"
endif

.DELETE_ON_ERROR:
# DO NOT DELETE
