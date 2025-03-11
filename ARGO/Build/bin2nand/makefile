EXEC_NAME_BIN2NAND  = bin2nand

.PHONY: all

all: clean bin

clean:
	@rm -f *.o
	@rm -f $(EXEC_NAME_BIN2NAND)  

bin: 
	@gcc -O3 -c \
		bin2nand.c \
		nand_image.c \
		nand_errors.c \
		bch_encoder.c \
		bch_global.c
	@gcc -o $(EXEC_NAME_BIN2NAND) \
		bin2nand.o \
		nand_image.o \
		nand_errors.o \
		bch_encoder.o \
		bch_global.o -lm
	@chmod 777 $(EXEC_NAME_BIN2NAND)
