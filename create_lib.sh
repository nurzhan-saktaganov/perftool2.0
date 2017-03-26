#!/bin/sh
rm list.o stack.o context_string.o context_types.o\
	register_context.o omp_dbg.o dvmh_omp_event.o\
	dvmh_omp_thread_info.o dvmh_omp_event_analyzer.o

icc -c list.c stack.c context_string.c context_types.c\
	register_context.c omp_dbg.c dvmh_omp_event.c\
	dvmh_omp_thread_info.c dvmh_omp_event_analyzer.c -openmp &&\
\
ar cr omp_dbg.lib list.o stack.o context_string.o context_types.o\
	 register_context.o omp_dbg.o dvmh_omp_event.o\
	dvmh_omp_thread_info.o dvmh_omp_event_analyzer.o

