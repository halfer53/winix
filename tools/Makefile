REFORMAT = reformat_srec
GEN_BIN = gen_bin_code
BIN_CODE = bin_code

tools: $(REFORMAT).class $(GEN_BIN) $(BIN_CODE)
$(REFORMAT).class $(GEN_BIN): $(REFORMAT).java $(GEN_BIN).c $(BIN_CODE).c
	javac $(REFORMAT).java
	gcc $(GEN_BIN).c -o $(GEN_BIN)
	gcc $(BIN_CODE).c -o $(BIN_CODE)

clean:
	$(Q)-rm -f $(OBJECT) $(ASSEMBLY)
ifeq ($(KBUILD_VERBOSE),0)
	@echo "RM \t TOOLS"
endif

	
	