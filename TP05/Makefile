.PHONY : clean all __clean__
INIT = out

#
# ! If using another target than '$EXEC'/'ALL' first launch target '$INIT'  to  create 'out' directory in which to put object (.o) files

#
# Compiler flags
CC = gcc
CFLAGS = #-lm -Wall #-Wuninitialized -Wfloat-equal -Wshadow -Wbad-function-cast -Wcast-qual -Wcast-align -Wconversion -Wunreachable-code -Wformat=2 -Winit-self -Wno-conversion -Wno-discarded-qualifiers -Wno-format-y2k -Wno-format-nonliteral 

#
# Debug build settings
#
DBGFLAGS = #-g -DDEBUG

SERV_EXEC = server
CLIENT_EXEC = client

EXEC = $(SERV_EXEC) $(CLIENT_EXEC)

SHARED_OBJS = out/functions.o out/optprsr.o out/util.o

CLIENT_OBJS = out/client.o $(SHARED_OBJS)
SERV_OBJS = out/server.o $(SHARED_OBJS)

OBJS = out/client.o $(SERV_OBJS) 


all: $(EXEC)


$(SERV_EXEC): $(INIT) $(SERV_OBJS)
	$(CC) $(SERV_OBJS) -o $(SERV_EXEC)

$(CLIENT_EXEC): $(INIT) $(CLIENT_OBJS)
	$(CC) $(CLIENT_OBJS) -o $(CLIENT_EXEC)


out/client.o: client.c functions.h optprsr.h util.h
	$(CC) -c client.c $(CFLAGS) $(DBGFLAGS) -o out/client.o

out/server.o: server.c functions.h util.h
	$(CC) -c server.c $(CFLAGS) $(DBGFLAGS) -o out/server.o


out/functions.o: functions.c functions.h util.h
	$(CC) -c functions.c $(CFLAGS) $(DBGFLAGS) -o out/functions.o


out/optprsr.o: optprsr.c optprsr.h util.h
	$(CC) -c optprsr.c $(CFLAGS) $(DBGFLAGS) -o out/optprsr.o


out/util.o: util.c util.h
	$(CC) -c util.c $(CFLAGS) $(DBGFLAGS) -o out/util.o



$(INIT): 
	mkdir out

__clean__:
	- @rm $(EXEC) *.out out/*.o out/*.out *.exe* 2> /dev/null

# supress error ouput because cleaning without having compiled "throws" ENOENT error, whereas
# we don't really care whether they were indeed files to delete or not
clean:
	@make __clean__ -s
