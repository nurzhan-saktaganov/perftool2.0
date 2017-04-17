#ifndef DVMH_OMP_THREAD_INFO_H
#define DVMH_OMP_THREAD_INFO_H

#include "dvmh_omp_event.h"

typedef struct _dvmh_omp_thread_info dvmh_omp_thread_info;

dvmh_omp_thread_info *dvmh_omp_thread_info_create();
void dvmh_omp_thread_info_destroy(dvmh_omp_thread_info *info);
void dvmh_omp_thread_info_event_occured(dvmh_omp_thread_info *info, dvmh_omp_event *e);
dvmh_omp_event *dvmh_omp_thread_info_event_finished(dvmh_omp_thread_info *info);
dvmh_omp_event *dvmh_omp_thread_info_get_active_event(dvmh_omp_thread_info *info);
int dvmh_omp_thread_info_is_alive(dvmh_omp_thread_info *info);

#endif
