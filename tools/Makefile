REFORMAT = reformat_srec
GEN_BIN = gen_bin_code
BIN_CODE = bin_code

all: $(REFORMAT).class $(GEN_BIN) $(BIN_CODE)
$(REFORMAT).class $(GEN_BIN): $(REFORMAT).java $(GEN_BIN).c $(BIN_CODE).c
	javac $(REFORMAT).java
	gcc $(GEN_BIN).c -o $(GEN_BIN)
	gcc $(BIN_CODE).c -o $(BIN_CODE)
	cp $(GEN_BIN) $(REFORMAT).class $(BIN_CODE) ../

clean:
	-rm -f $(GEN_BIN) $(REFORMAT).class $(BIN_CODE)

	
	