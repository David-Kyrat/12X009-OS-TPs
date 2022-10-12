#
# Compiler flags
#

CC = gcc
CFLAGS = -lm -Wall -Wuninitialized -Wfloat-equal -Wshadow -Wbad-function-cast -Wcast-qual -Wcast-align -Wconversion -Wunreachable-code -Wformat=2 -Winit-self 
#-Wno-conversin -Wno-implicit-function-declaration  -Wno-discarded-qualifiers -Wno-unknown-pragmas
OBJ = *.o

#
# Debug build settings
#
DBGFLAGS = -g -DDEBUG


getopt_example: examples/getopt_example.c
	$(CC) examples/getopt_example.c -lssl -lcrypto $(CFLAGS) $(DBGFLAGS) -o examples/getopt_example.out


openssl_example: examples/openssl_example.c
	$(CC) examples/openssl_example.c -lssl -lcrypto $(CFLAGS) $(DBGFLAGS) -o examples/openssl_example.out

optionparser: OptionParser.c OptionParser.h
	$(CC) OptionParser.c $(CFLAGS) $(DBGFLAGS) -o optionparser.out

all: main.c options.c digest.c
	$(CC) $(CFLAGS) $(DBGFLAGS)  -o digest main.c options.c digest.c -lssl -lcrypto

clean:
	rm *parser.out
