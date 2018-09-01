CC=icc
LDFLAGS=
BIN=bin
LIB=lib
SRC=src
INC=-I$(LIB) -I$(SRC)
STDC99=-std=c99
CFLAGS=-c -Wall $(INC) -fPIC $(STDC99)
OPENMP=-qopenmp

all: omp_dbg.so

omp_dbg.so: lib src
	$(CC) -Wall -shared -fPIC $(OPENMP) \
		-Wl,--version-script=exportmap.exp \
		-o $(BIN)/omp_dbg.so \
		$(BIN)/$(LIB)/list.o \
		$(BIN)/$(LIB)/stack.o \
		$(BIN)/$(LIB)/context_string.o \
		$(BIN)/$(SRC)/context_descriptor.o \
		$(BIN)/$(SRC)/register_context.o \
		$(BIN)/$(SRC)/dvmh_omp_interval.o \
		$(BIN)/$(SRC)/dvmh_omp_thread_context.o \
		$(BIN)/$(SRC)/dvmh_omp_runtime_context.o \
		$(BIN)/$(SRC)/omp_dbg.o

.PHONY: src
src: context_descriptor.o register_context.o dvmh_omp_interval.o dvmh_omp_thread_context.o dvmh_omp_runtime_context.o omp_dbg.o

omp_dbg.o: $(SRC)/omp_dbg.c
	$(CC) $(CFLAGS) $(OPENMP) $(SRC)/omp_dbg.c -o $(BIN)/$(SRC)/omp_dbg.o

dvmh_omp_runtime_context.o: $(SRC)/dvmh_omp_runtime_context.c dvmh_omp_thread_context.o
	$(CC) $(CFLAGS) $(OPENMP) $(SRC)/dvmh_omp_runtime_context.c -o $(BIN)/$(SRC)/dvmh_omp_runtime_context.o

dvmh_omp_thread_context.o: $(SRC)/dvmh_omp_thread_context.c dvmh_omp_interval.o
	$(CC) $(CFLAGS) $(OPENMP) $(SRC)/dvmh_omp_thread_context.c -o $(BIN)/$(SRC)/dvmh_omp_thread_context.o

dvmh_omp_interval.o: $(SRC)/dvmh_omp_interval.c
	$(CC) $(CFLAGS) $(OPENMP) $(SRC)/dvmh_omp_interval.c -o $(BIN)/$(SRC)/dvmh_omp_interval.o

register_context.o: lib context_descriptor.o $(SRC)/register_context.c
	$(CC) $(CFLAGS) $(SRC)/register_context.c -o $(BIN)/$(SRC)/register_context.o

context_descriptor.o: lib $(SRC)/context_descriptor.c
	$(CC) $(CFLAGS) $(SRC)/context_descriptor.c -o $(BIN)/$(SRC)/context_descriptor.o

.PHONY: lib
lib: list.o stack.o context_string.o

list.o: $(LIB)/list.c
	$(CC) $(CFLAGS) $(LIB)/list.c -o $(BIN)/$(LIB)/list.o

stack.o: $(LIB)/stack.c
	$(CC) $(CFLAGS) $(LIB)/stack.c -o $(BIN)/$(LIB)/stack.o

context_string.o: $(LIB)/context_string.c
	$(CC) $(CFLAGS) $(LIB)/context_string.c -o $(BIN)/$(LIB)/context_string.o

