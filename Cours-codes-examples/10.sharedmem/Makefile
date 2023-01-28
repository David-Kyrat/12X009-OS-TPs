CC=gcc
CC_OPTS= -Wall -g
LN_OPTS= -lrt

all: shm-create shm-help

shm-create: creator.c shm.h
	$(CC) $(CC_OPTS) creator.c -o shm-create $(LN_OPTS)

shm-help: helper.c shm.h
	$(CC) $(CC_OPTS) helper.c -o shm-help $(LN_OPTS)
