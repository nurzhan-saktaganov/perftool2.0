#!/bin/sh
rm list.o context_string.o context_types.o register_context.o omp_dbg.o
icc -c list.c context_string.c context_types.c register_context.c omp_dbg.c &&\
ar cr omp_dbg.lib list.o context_string.o context_types.o register_context.o omp_dbg.o
