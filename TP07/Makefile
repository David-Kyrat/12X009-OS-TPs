.PHONY : clean all init

# ! To Init workspace and allows makefile to compile correctly first launch target 'init'


# * Compiler flags

CC = gcc
CFLAGS = -lm -Wall -Wuninitialized -Wfloat-equal -Wshadow -Wbad-function-cast -Wcast-qual -Wcast-align -Wconversion -Wunreachable-code -Wformat=2 -Winit-self -Wno-conversion -Wno-discarded-qualifiers -Wno-format-y2k -Wno-format-nonliteral 
LFLAGS = -lrt -lpthread


# * Debug build settings

DBGFLAGS = -g -DDEBUG

CUISINIER_EXEC = cuisinier
SERVEUR_EXEC = serveur

EXEC = $(SERVEUR_EXEC) $(CUISINIER_EXEC)

SHARED_OBJS = out/cook.o

CUISINIER_OBJS = out/cuisinier.o $(SHARED_OBJS)
SERVEUR_OBJS = out/serveur.o $(SHARED_OBJS)


all: $(EXEC)

$(SERVEUR_EXEC): $(SERVEUR_OBJS)
	$(CC) $(SERVEUR_OBJS) -o $(SERVEUR_EXEC) $(LFLAGS)

$(CUISINIER_EXEC): $(CUISINIER_OBJS)
	$(CC) $(CUISINIER_OBJS) -o $(CUISINIER_EXEC) $(LFLAGS)

out/serveur.o: serveur.c
	$(CC) -c serveur.c $(CFLAGS) $(DBGFLAGS) -o out/serveur.o $(LFLAGS)

out/cuisinier.o: cuisinier.c
	$(CC) -c cuisinier.c $(CFLAGS) $(DBGFLAGS) -o out/cuisinier.o $(LFLAGS)

out/cook.o: cook.c cook.h
	$(CC) -c cook.c $(CFLAGS) $(DBGFLAGS) -o out/cook.o $(LFLAGS)



#! Make directory that will contain all .o & .out (except for main.out which will be at root of source dir i.e. where Makefile is)

init: out

out: 
	mkdir out

clean:
	rm *.out out/*.o out/*.out

