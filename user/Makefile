INCLUDE_DIR = ../include
INCLUDES = $(wildcard $(INCLUDE_DIR)/*.h $(INCLUDE_DIR)/sys/*.h )
LIBS_O = $(shell find ../lib -name "*.o")

SHELL_PROGS = shell shell_parse shell_test
PROGS = $(SHELL_PROGS)
SOURCES = $(PROGS:=.c)
ASSEMBLY = $(PROGS:=.s)
OBJECT = $(PROGS:=.o)

.PHONY : all

all: wramp
	@wlink -o shell.srec shell.o shell_test.o shell_parse.o $(LIBS_O)
	@echo "LD \t SHELL"

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
	@echo "RM \t USER"

.DELETE_ON_ERROR:
# DO NOT DELETE
