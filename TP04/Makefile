.PHONY : clean all init
#
# ! To Init workspace and allows makefile to compile correctly first launch target 'init'
#

#
# Compiler flags
#
CC = gcc
CFLAGS = -lm -Wall -Wuninitialized -Wfloat-equal -Wshadow -Wbad-function-cast -Wcast-qual -Wcast-align -Wconversion -Wunreachable-code -Wformat=2 -Winit-self -Wno-conversion -Wno-discarded-qualifiers -Wno-format-y2k

#
# Debug build settings
#
DBGFLAGS = -g -DDEBUG
EXEC = main.out
OBJS = out/main.o out/files.o out/optprsr.o out/util.o

all: $(EXEC)


$(EXEC): $(OBJS)
	$(CC) $(OBJS) -o $(EXEC)

debug: $(OBJS)
	$(CC) $(OBJS) -o main.out


out/main.o: main.c files.h optprsr.h util.h
	$(CC) -c main.c $(CFLAGS) $(DBGFLAGS) -o out/main.o


out/files.o: files.c files.h util.h
	$(CC) -c files.c $(CFLAGS) $(DBGFLAGS) -o out/files.o


out/optprsr.o: optprsr.c optprsr.h util.h
	$(CC) -c optprsr.c $(CFLAGS) $(DBGFLAGS) -o out/optprsr.o


out/util.o: util.c util.h
	$(CC) -c util.c $(CFLAGS) $(DBGFLAGS) -o out/util.o

#! Make directory that will contain all .o & .out (except for main.out wich will be at root of source dir i.e. where Makefile is)
#
init: out

out: 
	mkdir out

clean:
	rm *.out out/*.o out/*.out
