#ifndef DVMH_OMP_INTERVAL_H
#define DVMH_OMP_INTERVAL_H

#include "dvmh_omp_event.h"

typedef struct _dvmh_omp_interval dvmh_omp_interval;
typedef struct _dvmh_omp_subinterval_iterator dvmh_omp_subinterval_iterator;

dvmh_omp_interval *dvmh_omp_interval_build(dvmh_omp_event *e);
void dvmh_omp_interval_destroy(dvmh_omp_interval *i);

double dvmh_omp_interval_get_load_imbalance_time(dvmh_omp_interval *i);
double dvmh_omp_interval_get_thread_prod_max(dvmh_omp_interval *i);
double dvmh_omp_interval_get_thread_prod_min(dvmh_omp_interval *i);
double dvmh_omp_interval_get_thread_prod_avg(dvmh_omp_interval *i);

int dvmh_omp_interval_is_in_parallel(dvmh_omp_interval *i);
long dvmh_omp_interval_get_id(dvmh_omp_interval *i);
int dvmh_omp_interval_begin_line(dvmh_omp_interval *i);
char *dvmh_omp_interval_get_file_name(dvmh_omp_interval *i);

dvmh_omp_subinterval_iterator *dvmh_omp_subinterval_iterator_new(dvmh_omp_interval *i);
int dvmh_omp_subinterval_iterator_has_next(dvmh_omp_subinterval_iterator *it);
dvmh_omp_interval *dvmh_omp_subinterval_iterator_next(dvmh_omp_subinterval_iterator *it);
void dvmh_omp_subinterval_iterator_destroy(dvmh_omp_subinterval_iterator *it);

#endif
