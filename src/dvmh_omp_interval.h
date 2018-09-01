#ifndef DVMH_OMP_INTERVAL_H
#define DVMH_OMP_INTERVAL_H

#include <stdint.h>
#include <stdbool.h>

#include "list.h"

typedef struct _dvmh_omp_interval_t {
    uint64_t execution_count;
    int id;
    int parent_id; // TODO maybe we should remove this
    double io_time; // TODO must be aggregated. See perftool code.
    double sync_barrier_time; // TODO must be aggregated. See perftool code.
    double idle_critical_time; // TODO must be aggregated. See perftool code. Reconsider.
    double sync_flush_time; // TODO must be aggregated. See perftool code.
    double used_time; // TODO must be integrated. See perftool code.
    double execution_time;
    double idle_parallel_time;
    int used_threads_number; // TODO для каждого интервала считаем несредственно использованных. Потом интегрируем по вложенным.
    list *subintervals;
    double thread_prod_max;
    double thread_prod_min;
    double thread_prod_avg;
    double load_imbalance;
    // TODO store context_desciptor
    // TODO execution time
    // TODO threads num - can be post calculated
    // TODO idle parallel time - 
    // TODO total time - post calc
    // TODO productive time - can be post calculated
    // TODO insufficient parallelism - post calc
    // TODO efficiency - can be post calculated
    // TODO load imbalance
    // TODO lost time
} dvmh_omp_interval_t;


#define DVMH_OMP_INTERVAL_PARENT_UNDEFINED -1
#define DVMH_OMP_INTERVAL_NO_PARENT -2

// TODO maybe there should be interval init function?

// Setters
void
dvmh_omp_interval_init(
        dvmh_omp_interval_t *i);

void
dvmh_omp_interval_deinit(
        dvmh_omp_interval_t *i);

void
dvmh_omp_interval_set_id(
        dvmh_omp_interval_t *i,
        int id);

void
dvmh_omp_interval_set_parent_id(
        dvmh_omp_interval_t *i,
        int parent_id);

void
dvmh_omp_interval_add_io_time(
        dvmh_omp_interval_t *i,
        double io_time);

void
dvmh_omp_interval_add_barrier_time(
        dvmh_omp_interval_t *i,
        double barrier_time);

void
dvmh_omp_interval_add_critical_time(
        dvmh_omp_interval_t *i,
        double critical_time);

void
dvmh_omp_interval_add_flush_time(
        dvmh_omp_interval_t *i,
        double flush_time);

void
dvmh_omp_interval_add_used_time(
        dvmh_omp_interval_t *i,
        double used_time);

void
dvmh_omp_interval_add_execution_time(
        dvmh_omp_interval_t *i,
        double t);

void
dvmh_omp_interval_add_idle_parallel_time(
        dvmh_omp_interval_t *i,
        double t);

void
dvmh_omp_interval_add_exectuion_count(
        dvmh_omp_interval_t *i,
        uint64_t count);

void
dvmh_omp_interval_add_used_threads_num(
        dvmh_omp_interval_t *i,
        int n);

void
dvmh_omp_interval_set_thread_prod_avg(
        dvmh_omp_interval_t *i,
        double t);

void
dvmh_omp_interval_set_thread_prod_max(
        dvmh_omp_interval_t *i,
        double t);

void
dvmh_omp_interval_set_thread_prod_min(
        dvmh_omp_interval_t *i,
        double t);

void
dvmh_omp_interval_add_load_imbalance(
        dvmh_omp_interval_t *i,
        double t);

// Getters

int
dvmh_omp_interval_get_parent_id(
        dvmh_omp_interval_t *i);

int
dvmh_omp_interval_get_id(
        dvmh_omp_interval_t *i);

double
dvmh_omp_interval_total_time(
        dvmh_omp_interval_t *i);

double
dvmh_omp_interval_lost_time(
        dvmh_omp_interval_t *i);

double
dvmh_omp_interval_productive_time(
        dvmh_omp_interval_t *i);

double
dvmh_omp_interval_efficiency(
        dvmh_omp_interval_t *i);

double
dvmh_omp_interval_insufficient_parallelism(
        dvmh_omp_interval_t *i);

double
dvmh_omp_interval_io_time(
        dvmh_omp_interval_t *i);

int
dvmh_omp_interval_execution_count(
        dvmh_omp_interval_t *i);

double
dvmh_omp_interval_execution_time(
        dvmh_omp_interval_t *i);

double
dvmh_omp_interval_sync_barrier_time(
        dvmh_omp_interval_t *i);

double
dvmh_omp_interval_used_time(
        dvmh_omp_interval_t *i);

int
dvmh_omp_interval_used_threads_num(
        dvmh_omp_interval_t *i);

double
dvmh_omp_interval_idle_critical_time(
        dvmh_omp_interval_t *i);

double
dvmh_omp_interval_sync_flush_time(
        dvmh_omp_interval_t *i);

double
dvmh_omp_interval_idle_parallel_time(
        dvmh_omp_interval_t *i);

double
dvmh_omp_interval_load_imbalance_time(
        dvmh_omp_interval_t *i);

double
dvmh_omp_interval_thread_prod_max(
        dvmh_omp_interval_t *i);

double
dvmh_omp_interval_thread_prod_min(
        dvmh_omp_interval_t *i);

double
dvmh_omp_interval_thread_prod_avg(
        dvmh_omp_interval_t *i);

int
dvmv_omp_interval_is_in_parallel(
        dvmh_omp_interval_t *i);

// Subintervals API

typedef struct _dvmh_omp_subintervals_iterator_t {
    list_iterator *it;
} dvmh_omp_subintervals_iterator_t;

void
dvmh_omp_interval_add_subinterval(
        dvmh_omp_interval_t *i,
        dvmh_omp_interval_t *s);

bool
dvmh_omp_interval_has_subintervals(
        dvmh_omp_interval_t *i);

dvmh_omp_subintervals_iterator_t *
dvmh_omp_subintervals_iterator_new(
        dvmh_omp_interval_t *i);

bool
dvmh_omp_subintervals_iterator_has_next(
        dvmh_omp_subintervals_iterator_t *it);

dvmh_omp_interval_t *
dvmh_omp_subintervals_iterator_next(
        dvmh_omp_subintervals_iterator_t *it);

void
dvmh_omp_subintervals_iterator_destroy(
        dvmh_omp_subintervals_iterator_t *it);

#endif
