#!/usr/bin/env bash
icc -Wall -shared -qopenmp -fPIC\
	-Wl,--version-script=exportmap.exp\
	-o omp_dbg.so\
	list.c stack.c context_string.c context_descriptor.c\
	register_context.c omp_dbg.c dvmh_omp_event.c\
	dvmh_omp_thread_info.c dvmh_omp_event_analyzer.c\
	dvmh_omp_interval.c
