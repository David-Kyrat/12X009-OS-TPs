#
# Compiler flags
#
CC = gcc
CFLAGS = -lm -Wall -Wuninitialized -Wfloat-equal -Wshadow -Wbad-function-cast -Wcast-qual -Wcast-align -Wconversion -Wunreachable-code -Wformat=2 -Winit-self -Wno-conversion -Wno-discarded-qualifiers
EXEC = digest 
OBJS = out/hash_calc.o out/OptionParser.o
#
# Debug build settings
#
DBGFLAGS = -g -DDEBUG


all: $(EXEC)

$(EXEC): out/main.o $(OBJS)
	$(CC) out/main.o $(OBJS) -o $(EXEC) -lssl -lcrypto

main.out: out/main.o $(OBJS)
	$(CC) out/main.o $(OBJS) -o main.out -lssl -lcrypto


out/main.o: main.c hash_calc.h OptionParser.h
	$(CC) -c main.c $(CFLAGS) $(DBGFLAGS) -o out/main.o


test.out: out/test.o $(OBJS)
	$(CC) out/test.o $(OBJS) -o test.out -lssl -lcrypto


out/test.o: test.c hash_calc.h OptionParser.h
	$(CC) -c test.c $(CFLAGS) $(DBGFLAGS) -o out/test.o


out/hash_calc.o: hash_calc.c hash_calc.h OptionParser.h
	$(CC) -c hash_calc.c $(CFLAGS) $(DBGFLAGS) -o out/hash_calc.o


out/OptionParser.o: OptionParser.c OptionParser.h
	$(CC) -c OptionParser.c $(CFLAGS) $(DBGFLAGS) -o out/OptionParser.o


clean:
	- rm *.out out/*.o out/*.out
