.PHONY : clean all __clean__
INIT = out

#
# ! If using another target than '$EXEC'/'ALL' first launch target '$INIT'  to  create 'out' directory in which to put object (.o) files

#
# Compiler flags
#
CC = gcc
CFLAGS = -lm -Wall -Wuninitialized -Wfloat-equal -Wshadow -Wbad-function-cast -Wcast-qual -Wcast-align -Wconversion -Wunreachable-code -Wformat=2 -Winit-self -Wno-conversion -Wno-discarded-qualifiers -Wno-format-y2k -Wno-format-nonliteral 

#
# Debug build settings
#
DBGFLAGS = -g -DDEBUG
EXEC = main
OBJS = out/main.o out/lock.o out/inp.o out/files.o out/optprsr.o out/util.o

all: $(EXEC)

$(EXEC): $(INIT) $(OBJS)
	$(CC) $(OBJS) -o $(EXEC)

out/main.o: main.c lock.h inp.h files.h optprsr.h util.h
	$(CC) -c main.c $(CFLAGS) $(DBGFLAGS) -o out/main.o


out/lock.o: lock.c lock.h inp.h
	$(CC) -c lock.c $(CFLAGS) $(DBGFLAGS) -o out/lock.o


out/inp.o: inp.c inp.h util.h
	$(CC) -c inp.c $(CFLAGS) $(DBGFLAGS) -o out/inp.o


out/files.o: files.c files.h util.h
	$(CC) -c files.c $(CFLAGS) $(DBGFLAGS) -o out/files.o


out/optprsr.o: optprsr.c optprsr.h util.h
	$(CC) -c optprsr.c $(CFLAGS) $(DBGFLAGS) -o out/optprsr.o


out/util.o: util.c util.h
	$(CC) -c util.c $(CFLAGS) $(DBGFLAGS) -o out/util.o



$(INIT): 
	mkdir out

__clean__:
	- @rm $(EXEC) *.out out/*.o out/*.out 2> /dev/null

# supress error ouput because cleaning without having compiled "throws" ENOENT error, whereas
# we don't really care whether they were indeed files to delete or not
clean:
	@make __clean__ -s
