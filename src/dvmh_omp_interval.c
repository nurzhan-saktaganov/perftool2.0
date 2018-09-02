#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "list.h"
#include "dvmh_omp_interval.h"

void
dvmh_omp_interval_init(
        dvmh_omp_interval_t *i)
{
    assert(i != NULL);

    // zeroing all fields of struct
    memset(i, 1, sizeof(dvmh_omp_interval_t));
    i->id = DVMH_OMP_INTERVAL_ID_UNDEFINED;
    i->parent_id = DVMH_OMP_INTERVAL_ID_UNDEFINED;
}

void
dvmh_omp_interval_deinit(
        dvmh_omp_interval_t *i)
{
    assert(i != NULL);
    if (i->subintervals != NULL) {
        // just destroy connections
        list_destroy(i->subintervals);
        i->subintervals = NULL;
    }
}

void
dvmh_omp_interval_set_parent_id(
        dvmh_omp_interval_t *i,
        int parent_id)
{
    i->parent_id = parent_id;
}

void
dvmh_omp_interval_set_id(
        dvmh_omp_interval_t *i,
        int id)
{
    i->id = id;
}

void
dvmh_omp_interval_add_execution_count(
        dvmh_omp_interval_t *i,
        uint64_t count)
{
    i->execution_count += count;
}

void
dvmh_omp_interval_set_used_threads_num(
        dvmh_omp_interval_t *i,
        int n)
{
    i->used_threads_number += n;
}

void
dvmh_omp_interval_set_thread_prod_avg(
        dvmh_omp_interval_t *i,
        double t)
{
    i->thread_prod_avg = t;
}

void
dvmh_omp_interval_set_thread_prod_max(
        dvmh_omp_interval_t *i,
        double t)
{
    i->thread_prod_max = t;
}

void
dvmh_omp_interval_set_thread_prod_min(
        dvmh_omp_interval_t *i,
        double t)
{
    i->thread_prod_min = t;
}

void
dvmh_omp_interval_add_load_imbalance(
        dvmh_omp_interval_t *i,
        double t)
{
    i->load_imbalance += t;
}

void
dvmh_omp_interval_add_io_time(
        dvmh_omp_interval_t *i,
        double io_time)
{
    i->io_time += io_time;
}

void
dvmh_omp_interval_add_barrier_time(
        dvmh_omp_interval_t *i,
        double barrier_time)
{
    i->sync_barrier_time += barrier_time;
}

void
dvmh_omp_interval_add_critical_time(
        dvmh_omp_interval_t *i,
        double critical_time)
{
    i->idle_critical_time += critical_time;
}

void
dvmh_omp_interval_add_flush_time(
        dvmh_omp_interval_t *i,
        double flush_time)
{
    i->sync_flush_time += flush_time;
}

void
dvmh_omp_interval_add_used_time(
        dvmh_omp_interval_t *i,
        double used_time)
{
    i->used_time += used_time;
}

void
dvmh_omp_interval_add_execution_time(
        dvmh_omp_interval_t *i,
        double t)
{
    i->execution_time += t;
}

void
dvmh_omp_interval_add_idle_parallel_time(
        dvmh_omp_interval_t *i,
        double t)
{
    i->idle_parallel_time += t;
}

// Getters

int
dvmh_omp_interval_get_parent_id(
        dvmh_omp_interval_t *i)
{
    assert(i != NULL);
    return i->parent_id;
}

int
dvmh_omp_interval_get_id(
        dvmh_omp_interval_t *i)
{
    assert(i != NULL);
    return i->id;
}

double
dvmh_omp_interval_total_time(
        dvmh_omp_interval_t *i)
{
    return i->execution_time * i->used_threads_number;
}

double
dvmh_omp_interval_lost_time(
        dvmh_omp_interval_t *i)
{
    return 0.0
        + i->sync_barrier_time
        + i->idle_critical_time
        + i->sync_flush_time
        + dvmh_omp_interval_insufficient_parallelism(i);
}

double
dvmh_omp_interval_productive_time(
        dvmh_omp_interval_t *i)
{
    return i->used_time
        - i->sync_barrier_time
        - i->idle_critical_time
        - i->sync_flush_time;
}

double
dvmh_omp_interval_efficiency(
        dvmh_omp_interval_t *i)
{
    return dvmh_omp_interval_productive_time(i) / dvmh_omp_interval_total_time(i);
}


double
dvmh_omp_interval_insufficient_parallelism(
        dvmh_omp_interval_t *i)
{
    return dvmh_omp_interval_total_time(i) - i->used_time;
}

double
dvmh_omp_interval_io_time(
        dvmh_omp_interval_t *i)
{
    return i->io_time;
}

int
dvmh_omp_interval_execution_count(
        dvmh_omp_interval_t *i)
{
    return i->execution_count;
}

double
dvmh_omp_interval_execution_time(
        dvmh_omp_interval_t *i)
{
    return i->execution_time;
}

double
dvmh_omp_interval_sync_barrier_time(
        dvmh_omp_interval_t *i)
{
    return i->sync_barrier_time;
}

double
dvmh_omp_interval_used_time(
        dvmh_omp_interval_t *i)
{
    return i->used_time;
}

int
dvmh_omp_interval_used_threads_num(
        dvmh_omp_interval_t *i)
{
    return i->used_threads_number;
}

double
dvmh_omp_interval_idle_critical_time(
        dvmh_omp_interval_t *i)
{
    return i->idle_critical_time;
}

double
dvmh_omp_interval_sync_flush_time(
        dvmh_omp_interval_t *i)
{
    return i->sync_flush_time;
}

double
dvmh_omp_interval_idle_parallel_time(
        dvmh_omp_interval_t *i)
{
    return i->idle_parallel_time;
}

double
dvmh_omp_interval_load_imbalance_time(
        dvmh_omp_interval_t *i)
{
    return i->load_imbalance;
}

double
dvmh_omp_interval_thread_prod_max(
        dvmh_omp_interval_t *i)
{
    return i->thread_prod_max;
}

double
dvmh_omp_interval_thread_prod_min(
        dvmh_omp_interval_t *i)
{
    return i->thread_prod_min;
}

double
dvmh_omp_interval_thread_prod_avg(
        dvmh_omp_interval_t *i)
{
    return i->thread_prod_avg;
}

int
dvmh_omp_interval_is_in_parallel(
        dvmh_omp_interval_t *i)
{
    return dvmh_omp_interval_used_threads_num(i) > 1;
}

bool
dvmh_omp_interval_has_been_executed(
        dvmh_omp_interval_t *i)
{
    return dvmh_omp_interval_used_time(i) > 0.0;
}

// Subintervals API

void
dvmh_omp_interval_add_subinterval(
        dvmh_omp_interval_t *i,
        dvmh_omp_interval_t *s)
{
    assert(i != NULL);
    assert(s != NULL);
    if (i->subintervals == NULL) {
        i->subintervals = list_create();
    }
    list_append_tail(i->subintervals, s);
}

bool
dvmh_omp_interval_has_subintervals(
        dvmh_omp_interval_t *i)
{
    assert(i != NULL);
    return i->subintervals != NULL;
}

dvmh_omp_subintervals_iterator_t *
dvmh_omp_subintervals_iterator_new(
        dvmh_omp_interval_t *i)
{
    assert(i != NULL);
    return (dvmh_omp_subintervals_iterator_t *) list_iterator_new(i->subintervals);
}

bool
dvmh_omp_subintervals_iterator_has_next(
        dvmh_omp_subintervals_iterator_t *it)
{
    assert(it != NULL);
    return list_iterator_has_next((list_iterator *) it);
}

dvmh_omp_interval_t *
dvmh_omp_subintervals_iterator_next(
        dvmh_omp_subintervals_iterator_t *it)
{
    assert(it != NULL);
    return (dvmh_omp_interval_t *) list_iterator_next((list_iterator *) it);
}

void
dvmh_omp_subintervals_iterator_destroy(
        dvmh_omp_subintervals_iterator_t *it)
{
    assert(it != NULL);
    list_iterator_destroy(it);
}
