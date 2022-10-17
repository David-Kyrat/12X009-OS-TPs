#
# Compiler flags
#
CC = gcc
CFLAGS = -lm -Wall -Wuninitialized -Wfloat-equal -Wshadow -Wbad-function-cast -Wcast-qual -Wcast-align -Wconversion -Wunreachable-code -Wformat=2 -Winit-self -Wno-conversion

#
# Debug build settings
#
DBGFLAGS = -g -DDEBUG


all: out/main.o out/hash_calc.o out/OptionParser.o
	$(CC) out/main.o out/hash_calc.o  out/OptionParser.o -o digest -lssl -lcrypto


out/main.o: main.c hash_calc.h OptionParser.h
	$(CC) -c main.c $(CFLAGS) $(DBGFLAGS) -o out/main.o

main.out: out/main.o out/hash_calc.o out/OptionParser.o
	$(CC) out/main.o out/hash_calc.o  out/OptionParser.o -o main.out -lssl -lcrypto


out/hash_calc.o: hash_calc.c hash_calc.h
	$(CC) -c hash_calc.c $(CFLAGS) $(DBGFLAGS) -o out/hash_calc.o


out/OptionParser.o: OptionParser.c OptionParser.h
	$(CC) -c OptionParser.c $(CFLAGS) $(DBGFLAGS) -o out/OptionParser.o

OptionParser.out: out/OptionParser.o
	$(CC) out/OptionParser.o -o out/OptionParser.out


getopt_example: examples/getopt_example.c
	$(CC) examples/getopt_example.c -lssl -lcrypto $(CFLAGS) $(DBGFLAGS) -o examples/getopt_example.out

openssl_example: examples/openssl_example.c
	$(CC) examples/openssl_example.c -lssl -lcrypto $(CFLAGS) $(DBGFLAGS) -o examples/openssl_example.out


clean:
	rm *.out out/*.o out/*.out
