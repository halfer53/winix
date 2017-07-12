REFORMAT = reformat_srec
GEN_BIN = gen_bin_code

all: $(REFORMAT).class $(GEN_BIN)
$(REFORMAT).class $(GEN_BIN): $(REFORMAT).java $(GEN_BIN).c
	javac $(REFORMAT).java
	gcc $(GEN_BIN).c -o $(GEN_BIN)
	cp $(GEN_BIN) $(REFORMAT).class ../

clean:
	-rm -f $(GEN_BIN) $(REFORMAT).class

	
	