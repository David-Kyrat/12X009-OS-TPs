CC=gcc
CCOPT=-Wall

all: standard fifo install

standard: standard.c
	$(CC) $(CCOPT) standard.c -o standard

fifo: FIFO.c
	$(CC) $(CCOPT) FIFO.c -o fifo -lrt

install:
	chown root ./fifo
	chmod u+s ./fifo
