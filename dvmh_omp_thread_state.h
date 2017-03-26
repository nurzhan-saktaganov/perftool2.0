#ifndef DVMH_OMP_THREAD_STATE_H
#define DVMH_OMP_THREAD_STATE_H

#include "stack.h"

typedef struct _dvmh_omp_thread_info {
	stack *active_events;
} dvmh_omp_thread_info;

dvmh_omp_thread_info *dvmh_omp_thread_info_create();

void dvmh_omp_thread_info_destroy(dvmh_omp_thread_info *info);

void dvmh_omp_thread_info_event_occured(
		dvmh_omp_thread_info *info,
		dvmh_omp_event *event);

void dvmh_omp_thread_info_event_finished(
		dvmh_omp_thread_info *info,
		dvmh_omp_event *event);

int dvmh_omp_thread_info_is_alive(dvmh_omp_thread_info *info);

#endif
