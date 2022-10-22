.PHONY : clean all
#
# Compiler flags
#
CC = gcc
CFLAGS = -lm -Wall -Wuninitialized -Wfloat-equal -Wshadow -Wbad-function-cast -Wcast-qual -Wcast-align -Wconversion -Wunreachable-code -Wformat=2 -Winit-self -Wno-conversion

#
# Debug build settings
#
DBGFLAGS = -g -DDEBUG

listDir.out: 
	$(CC) listDir.c $(CFLAGS) $(DBGFLAGS) -o listDir.out

all: 
	$(CC) main.c -o ultra-cp

ultra-cp: all

out/main.o: main.c #hash_calc.h OptionParser.h
	$(CC) -c main.c $(CFLAGS) $(DBGFLAGS) -o out/main.o

main.out: out/main.o out/hash_calc.o out/OptionParser.o
	$(CC)  -o main.out 



#out/OptionParser.o: OptionParser.c OptionParser.h
	#$(CC) -c OptionParser.c $(CFLAGS) $(DBGFLAGS) -o out/OptionParser.o

#OptionParser.out: out/OptionParser.o
#	$(CC) out/OptionParser.o -o out/OptionParser.out


clean:
	rm *.out out/*.o out/*.out
