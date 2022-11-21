.PHONY : clean all init
#
# ! To Init workspace and allows makefile to compile correctly first launch target 'init'

#
# Compiler flags
#
CC = gcc
CFLAGS = -lm -Wall -Wuninitialized -Wfloat-equal -Wshadow -Wbad-function-cast -Wcast-qual -Wcast-align -Wconversion -Wunreachable-code -Wformat=2 -Winit-self -Wno-conversion -Wno-discarded-qualifiers -Wno-format-y2k -Wno-format-nonliteral 

#
# Debug build settings
#
DBGFLAGS = -g -DDEBUG

SERV_EXEC = server.out
CLIENT_EXEC = client.out
EXEC = $(SERV_EXEC) $(CLIENT_EXEC)

SHARED_OBJS = out/functions.o out/files.o out/optprsr.o out/util.o

CLIENT_OBJS = out/client.o $(SHARED_OBJS)
SERV_OBJS = out/server.o $(SHARED_OBJS)

OBJS = out/client.o $(SERV_OBJS) 
#out/main.o out/files.o out/optprsr.o out/util.o


all: $(EXEC)


$(SERV_EXEC): $(SERV_OBJS)
	$(CC) $(SERV_OBJS) -o $(SERV_EXEC)

$(CLIENT_EXEC): $(CLIENT_OBJS)
	$(CC) $(CLIENT_OBJS) -o $(CLIENT_EXEC)


test: test.c $(SHARED_OBJS)
	$(CC) test.c $(SHARED_OBJS) $(CFLAGS) $(DBGFLAGS) -o test.out


out/client.o: client.c functions.h optprsr.h files.h util.h
	$(CC) -c client.c $(CFLAGS) $(DBGFLAGS) -o out/client.o

out/server.o: server.c functions.h optprsr.h files.h util.h
	$(CC) -c server.c $(CFLAGS) $(DBGFLAGS) -o out/server.o


out/functions.o: functions.c functions.h files.h util.h
	$(CC) -c functions.c $(CFLAGS) $(DBGFLAGS) -o out/functions.o


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


#debugs: $(SERV_OBJS)
#	$(CC) $(SERV_OBJS) -o server.out
#debugc: $(CLIENT_OBJS)
#	$(CC) $(CLIENT_OBJS) -o client.out
#out/main.o: main.c files.h optprsr.h util.h
#	$(CC) -c main.c $(CFLAGS) $(DBGFLAGS) -o out/main.o
