#!/bin/sh
rm list.o context_string.o context_types.o register_context.o omp_dbg.o dvmh_omp_event.o
icc -c list.c context_string.c context_types.c register_context.c omp_dbg.c dvmh_omp_event.c -openmp &&\
ar cr omp_dbg.lib list.o context_string.o context_types.o register_context.o omp_dbg.o dvmh_omp_event.o
