ifeq ($(INTEL_COMPILER), 1)
	CC=icc
	STDC99=-std=c99
	OPENMP=-qopenmp
	EXPORTMAP=-Wl,--version-script=exportmap.exp
else
	CC=gcc
	STDC99=--std=c99
	OPENMP=-fopenmp
	EXPORTMAP=
endif

LDFLAGS=
BIN=bin
LIB=lib
SRC=src
INC=-I$(LIB) -I$(SRC)

CFLAGS=-c -Wall $(INC) -fPIC $(STDC99)

ifeq ($(PERFTOOL_1_COMPATIBILIY), 1)
	CFLAGS += -DPERFTOOL_1_COMPATIBILIY
endif

all: directories omp_dbg.so

omp_dbg.so: lib src
	$(CC) -Wall -shared -fPIC $(OPENMP) \
		$(EXPORTMAP) \
		-o $(BIN)/omp_dbg.so \
		$(BIN)/$(LIB)/list.o \
		$(BIN)/$(LIB)/context_string.o \
		$(BIN)/$(SRC)/context_descriptor.o \
		$(BIN)/$(SRC)/register_context.o \
		$(BIN)/$(SRC)/dvmh_omp_interval.o \
		$(BIN)/$(SRC)/dvmh_omp_thread_context.o \
		$(BIN)/$(SRC)/dvmh_omp_runtime_context.o \
		$(BIN)/$(SRC)/omp_dbg.o

.PHONY: directories src
src: context_descriptor.o register_context.o dvmh_omp_interval.o dvmh_omp_thread_context.o dvmh_omp_runtime_context.o omp_dbg.o

directories:
	mkdir -p $(BIN)/$(SRC)
	mkdir -p $(BIN)/$(LIB)

omp_dbg.o: $(SRC)/omp_dbg.c
	$(CC) $(CFLAGS) $(OPENMP) $(SRC)/omp_dbg.c -o $(BIN)/$(SRC)/omp_dbg.o

dvmh_omp_runtime_context.o: $(SRC)/dvmh_omp_runtime_context.c dvmh_omp_thread_context.o
	$(CC) $(CFLAGS) $(OPENMP) $(SRC)/dvmh_omp_runtime_context.c -o $(BIN)/$(SRC)/dvmh_omp_runtime_context.o

dvmh_omp_thread_context.o: $(SRC)/dvmh_omp_thread_context.c dvmh_omp_interval.o
	$(CC) $(CFLAGS) $(SRC)/dvmh_omp_thread_context.c -o $(BIN)/$(SRC)/dvmh_omp_thread_context.o

dvmh_omp_interval.o: $(SRC)/dvmh_omp_interval.c
	$(CC) $(CFLAGS) $(SRC)/dvmh_omp_interval.c -o $(BIN)/$(SRC)/dvmh_omp_interval.o

register_context.o: lib context_descriptor.o $(SRC)/register_context.c
	$(CC) $(CFLAGS) $(SRC)/register_context.c -o $(BIN)/$(SRC)/register_context.o

context_descriptor.o: lib $(SRC)/context_descriptor.c
	$(CC) $(CFLAGS) $(SRC)/context_descriptor.c -o $(BIN)/$(SRC)/context_descriptor.o

.PHONY: lib
lib: list.o context_string.o

list.o: $(LIB)/list.c
	$(CC) $(CFLAGS) $(LIB)/list.c -o $(BIN)/$(LIB)/list.o

context_string.o: $(LIB)/context_string.c
	$(CC) $(CFLAGS) $(LIB)/context_string.c -o $(BIN)/$(LIB)/context_string.o

