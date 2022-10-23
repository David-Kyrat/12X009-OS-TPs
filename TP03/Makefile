.PHONY : clean all
#
# Compiler flags
#
CC = gcc
CFLAGS = -std=c17 -D_BSD_SOURCE -lm -Wall -Wuninitialized -Wfloat-equal -Wshadow -Wbad-function-cast -Wcast-qual -Wcast-align -Wconversion -Wunreachable-code -Wformat=2 -Winit-self -Wno-conversion

#
# Debug build settings
#
DBGFLAGS = -g -DDEBUG
EXEC = ultra-cp
OBJS = out/main.o out/optprsr.o out/util.o

all: $(EXEC)


$(EXEC): $(OBJS)
	$(CC) main.c $(CFLAGS) $(DBGFLAGS) -o ultra-cp


out/main.o: main.c #hash_calc.h OptionParser.h
	$(CC) -c main.c $(CFLAGS) $(DBGFLAGS) -o out/main.o

main.out: out/main.o out/hash_calc.o out/OptionParser.o
	$(CC)  -o main.out 


out/optprsr.o: optprsr.c optprsr.h
	$(CC) -c optprsr.c $(CFLAGS) $(DBGFLAGS) -o out/optprsr.o

out/util.o: util.c util.h
	$(CC) -c util.c $(CFLAGS) $(DBGFLAGS) -o out/util.o


#OptionParser.out: out/OptionParser.o
#	$(CC) out/OptionParser.o -o out/OptionParser.out


clean:
	rm *.out out/*.o out/*.out
