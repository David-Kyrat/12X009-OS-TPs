.PHONY : clean all init
#
# ! To Init workspace and allows makefile to compile correctly first launch target 'init'

#
# Compiler flags
#
CC = gcc
CFLAGS = -lm -Wall -Wuninitialized -Wfloat-equal -Wshadow -Wbad-function-cast -Wcast-qual -Wcast-align -Wconversion -Wunreachable-code -Wformat=2 -Winit-self -Wno-conversion -Wno-discarded-qualifiers -Wno-format-y2k -Wno-format-nonliteral -Wno-comment

#
# Debug build settings
#
DBGFLAGS = -g -DDEBUG
#
# Name of the final executable
#
EXEC = main.out
#
# All headers. i.e. dependency of the out/main.o target
#
HDRS = shell.h input.h files.h util.h

OBJS = out/main.o out/shell.o out/input.o out/files.o out/util.o


all: $(EXEC)


$(EXEC): $(OBJS)
	$(CC) $(OBJS) -o $(EXEC) -lreadline


test: test.c $(OBJS)
	$(CC) test.c out/shell.o out/input.o out/util.o $(CFLAGS) $(DBGFLAGS) -o test.out


main: $(OBJS)
	$(CC) $(OBJS) -o main -lreadline


out/main.o: main.c $(HDRS)
	$(CC) -c main.c $(CFLAGS) $(DBGFLAGS) -o out/main.o


out/shell.o: shell.c shell.h files.h util.h
	$(CC) -c shell.c $(CFLAGS) $(DBGFLAGS) -o out/shell.o


out/input.o: input.c input.h util.h
	$(CC) -c input.c $(CFLAGS) $(DBGFLAGS) -o out/input.o


out/files.o: files.c files.h util.h
	$(CC) -c files.c $(CFLAGS) $(DBGFLAGS) -o out/files.o


out/util.o: util.c util.h
	$(CC) -c util.c $(CFLAGS) $(DBGFLAGS) -o out/util.o


#
#! Make directory that will contain all .o & .out (except for main.out wich will be at root of source dir i.e. where Makefile is)
#
init: out

out: 
	mkdir out

clean:
	- @rm *.out out/*.o


#out/old-shell.o: old-shell.c old-shell.h files.h input.h util.h
#	$(CC) -c old-shell.c $(CFLAGS) $(DBGFLAGS) -o out/old-shell.o

#debugs: $(SERV_OBJS)
#	$(CC) $(SERV_OBJS) -o server.out
#debugc: $(CLIENT_OBJS)
#	$(CC) $(CLIENT_OBJS) -o client.out
#out/main.o: main.c files.h optprsr.h util.h
#	$(CC) -c main.c $(CFLAGS) $(DBGFLAGS) -o out/main.o
