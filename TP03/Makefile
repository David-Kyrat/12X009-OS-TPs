.PHONY : clean all
#
# Compiler flags
#
CC = gcc
CFLAGS = -lm -Wall -Wuninitialized -Wfloat-equal -Wshadow -Wbad-function-cast -Wcast-qual -Wcast-align -Wconversion -Wunreachable-code -Wformat=2 -Winit-self -Wno-conversion -Wno-discarded-qualifiers -Wno-format-y2k

#
# Debug build settings
#
DBGFLAGS = -g -DDEBUG
EXEC = ultra-cp
OBJS = out/main.o out/copy.o out/files.o out/optprsr.o out/util.o

all: $(EXEC)


$(EXEC): $(OBJS)
	$(CC) $(OBJS) -o ultra-cp

copy.out: out/copy.o out/util.o
	$(CC) out/copy.o out/util.o -o copy.out


out/main.o: main.c files.h copy.h optprsr.h util.h
	$(CC) -c main.c $(CFLAGS) $(DBGFLAGS) -o out/main.o


out/files.o: files.c files.h copy.h util.h
	$(CC) -c files.c $(CFLAGS) $(DBGFLAGS) -o out/files.o


out/copy.o: copy.c copy.h util.h
	$(CC) -c copy.c $(CFLAGS) $(DBGFLAGS) -o out/copy.o


out/optprsr.o: optprsr.c optprsr.h util.h
	$(CC) -c optprsr.c $(CFLAGS) $(DBGFLAGS) -o out/optprsr.o


out/util.o: util.c util.h
	$(CC) -c util.c $(CFLAGS) $(DBGFLAGS) -o out/util.o


clean:
	rm *.out out/*.o out/*.out
