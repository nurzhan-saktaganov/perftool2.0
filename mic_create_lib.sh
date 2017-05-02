#!/bin/sh
rm list.o stack.o context_string.o context_descriptor.o\
	register_context.o omp_dbg.o dvmh_omp_event.o\
	dvmh_omp_thread_info.o dvmh_omp_event_analyzer.o\
	dvmh_omp_interval.o

icc -Wall -c list.c stack.c context_string.c context_descriptor.c\
	register_context.c omp_dbg.c dvmh_omp_event.c\
	dvmh_omp_thread_info.c dvmh_omp_event_analyzer.c\
	dvmh_omp_interval.c -mmic	 -qopenmp &&\
\
/opt/mpss/3.6.1/sysroots/x86_64-mpsssdk-linux/usr/libexec/k1om-mpss-linux/gcc/k1om-mpss-linux/5.1.1/ar\
 cr omp_dbg.lib list.o stack.o context_string.o context_descriptor.o\
	register_context.o omp_dbg.o dvmh_omp_event.o\
	dvmh_omp_thread_info.o dvmh_omp_event_analyzer.o\
	dvmh_omp_interval.o

