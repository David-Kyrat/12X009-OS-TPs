.PHONY : clean all __clean__
INIT = out

#
# ! If using another target than '$EXEC'/'ALL' first launch target '$INIT'  to  create 'out' directory in which to put object (.o) files

#
# Compiler flags
#
CC = gcc
CFLAGS = -lm -Wall -Wuninitialized -Wfloat-equal  -Wconversion -Wunreachable-code -Wformat=2 -Winit-self -Wno-conversion -Wno-discarded-qualifiers -Wno-format-y2k -Wno-format-nonliteral -Wno-comment -Wno-incompatible-pointer-types

#
# Debug build settings
#
DBGFLAGS = -g -DDEBUG
#
# Name of the final executable
#
EXEC = shell
#
# All headers. i.e. dependency of the out/main.o target
#
HDRS = shell.h input.h files.h util.h

OBJS = out/main.o out/shell.o out/input.o out/files.o out/util.o


all: $(EXEC)


$(EXEC): $(INIT) $(OBJS)
	$(CC) $(OBJS) -o $(EXEC) -lreadline


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



$(INIT): 
	mkdir out

__clean__:
	- @rm $(EXEC) *.out out/*.o out/*.out 2> /dev/null

# supress error ouput because cleaning without having compiled "throws" ENOENT error, whereas
# we don't really care whether they were indeed files to delete or not
clean:
	@make __clean__ -s