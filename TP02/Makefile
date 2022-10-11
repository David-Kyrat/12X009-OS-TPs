#
# Compiler flags
#

CC = gcc
CFLAGS = -lm -Wall -Wuninitialized -Wfloat-equal -Wshadow -Wbad-function-cast \
-Wcast-qual -Wcast-align -Wconversion -Wunreachable-code -Wformat=2 -Winit-self 
#-Wno-conversion -Wno-implicit-function-declaration  -Wno-discarded-qualifiers -Wno-unknown-pragmas
OBJ = .

#
# Debug build settings
#
DBGFLAGS = -g -DDEBUG

all: main.c options.c digest.c
	$(CC) -o digest main.c options.c digest.c -lssl -lcrypto
