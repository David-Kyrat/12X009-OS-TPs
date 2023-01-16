.PHONY : clean all $(INIT) __clean__
INIT = out


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

$(EXEC): init out/main.o $(OBJS)
	$(CC) out/main.o $(OBJS) -o $(EXEC) -lssl -lcrypto


out/main.o: main.c hash_calc.h OptionParser.h
	$(CC) -c main.c $(CFLAGS) $(DBGFLAGS) -o out/main.o


out/hash_calc.o: hash_calc.c hash_calc.h OptionParser.h
	$(CC) -c hash_calc.c $(CFLAGS) $(DBGFLAGS) -o out/hash_calc.o


out/OptionParser.o: OptionParser.c OptionParser.h
	$(CC) -c OptionParser.c $(CFLAGS) $(DBGFLAGS) -o out/OptionParser.o



$(INIT): 
	mkdir out

__clean__:
	- @rm $(EXEC) *.out out/*.o out/*.out 2> /dev/null

# supress error ouput because cleaning without having compiled "throws" ENOENT error, whereas
# we don't really care whether they were indeed files to delete or not
clean:
	@make __clean__ -s
