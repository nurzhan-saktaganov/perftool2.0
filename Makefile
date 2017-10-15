CC=gcc
LDFLAGS=
BIN=bin
LIB=lib
INC=-I$(LIB)
CFLAGS=-c -Wall $(INC)

all: lib

lib: list.o stack.o context_string.o

list.o: $(LIB)/list.c
	$(CC) $(CFLAGS) $(LIB)/list.c -o $(BIN)/$(LIB)/list.o

stack.o: $(LIB)/stack.c
	$(CC) $(CFLAGS) $(LIB)/stack.c -o $(BIN)/$(LIB)/stack.o

context_string.o: $(LIB)/context_string.c
	$(CC) $(CFLAGS) $(LIB)/context_string.c -o $(BIN)/$(LIB)/context_string.o

