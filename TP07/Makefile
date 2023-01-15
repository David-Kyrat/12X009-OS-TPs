.PHONY : clean all init __clean__

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

CUISINIER_OBJS = out/cuisinier.o
SERVEUR_OBJS = out/serveur.o


all: init $(EXEC)

$(SERVEUR_EXEC): $(SERVEUR_OBJS)
	$(CC) $(SERVEUR_OBJS) -o $(SERVEUR_EXEC) $(LFLAGS)

$(CUISINIER_EXEC): $(CUISINIER_OBJS)
	$(CC) $(CUISINIER_OBJS) -o $(CUISINIER_EXEC) $(LFLAGS)

out/serveur.o: serveur.c
	$(CC) -c serveur.c $(CFLAGS) $(DBGFLAGS) -o out/serveur.o $(LFLAGS)

out/cuisinier.o: cuisinier.c
	$(CC) -c cuisinier.c $(CFLAGS) $(DBGFLAGS) -o out/cuisinier.o $(LFLAGS)



init: out

out: 
	mkdir out

__clean__:
	- @rm $(EXEC) *.out out/*.o out/*.out 2> /dev/null

# supress error ouput because cleaning without having compiled "throws" ENOENT error, whereas
# we don't really care whether they were indeed files to delete or not
clean:
	@make __clean__ -s
