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
EXEC = ultra-cp
OBJS = out/main.o out/optprsr.o out/util.o

all: $(EXEC)


$(EXEC): $(OBJS)
	$(CC) $(OBJS) -o ultra-cp


out/main.o: main.c optprsr.h util.h
	$(CC) -c main.c $(CFLAGS) $(DBGFLAGS) -o out/main.o

out/optprsr.o: optprsr.c optprsr.h util.h
	$(CC) -c optprsr.c $(CFLAGS) $(DBGFLAGS) -o out/optprsr.o


out/util.o: util.c util.h
	$(CC) -c util.c $(CFLAGS) $(DBGFLAGS) -o out/util.o


#OptionParser.out: out/OptionParser.o
#	$(CC) out/OptionParser.o -o out/OptionParser.out


clean:
	rm *.out out/*.o out/*.out
