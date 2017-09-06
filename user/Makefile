INCLUDE_DIR = ../include
INCLUDES = $(wildcard $(INCLUDE_DIR)/*.h $(INCLUDE_DIR)/sys/*.h )
LIBS_O = $(shell find ../lib -name "*.o")

SHELL_PROGS = shell shell_parse shell_test
PROGS = $(SHELL_PROGS)
SOURCES = $(PROGS:=.c)
ASSEMBLY = $(PROGS:=.s)
OBJECT = $(PROGS:=.o)

.PHONY : all

shell: user
	@wlink -o shell.srec shell.o shell_test.o shell_parse.o $(LIBS_O)
	@echo "LD \t SHELL"

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
	makedepend -I$(INCLUDE_DIR) $(SOURCES)

clean:
	$(Q)-rm -f $(OBJECT) $(ASSEMBLY)
ifeq ($(KBUILD_VERBOSE),0)
	@echo "RM \t USER"
endif

.DELETE_ON_ERROR:
# DO NOT DELETE
