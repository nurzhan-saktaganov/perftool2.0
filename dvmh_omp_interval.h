#ifndef DVMH_OMP_INTERVAL_H
#define DVMH_OMP_INTERVAL_H

#include "dvmh_omp_event.h"

typedef struct _events_occurrences events_occurrences;
typedef struct _dvmh_omp_interval dvmh_omp_interval;

dvmh_omp_interval *dvmh_omp_interval_build(dvmh_omp_event *e);
void dvmh_omp_interval_destroy(dvmh_omp_interval *i);

#endif
