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
    memset((void *) i, 1, sizeof(dvmh_omp_interval_t));
}

void
dvmh_omp_interval_deinit(
        dvmh_omp_interval_t *i)
{
    assert(i != NULL);
    if (i->subintervals != NULL) {
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
dvmh_omp_interval_add_exectuion_count(
        dvmh_omp_interval_t *i,
        uint64_t count)
{
    i->execution_count += count;
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

double
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

void
dvmh_omp_interval_integrate(
        dvmh_omp_interval_t **from,
        int num_threads,
        int size,
        dvmh_omp_interval_t *into)
{
    // TODO
    return;
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
    dvmh_omp_subintervals_iterator_t *it =
            (dvmh_omp_subintervals_iterator_t *) malloc(sizeof(dvmh_omp_subintervals_iterator_t ));
    assert(it != NULL);
    it->it = list_iterator_new(i->subintervals);
    return it;
}

bool
dvmh_omp_subintervals_iterator_has_next(
        dvmh_omp_subintervals_iterator_t *it)
{
    assert(it != NULL);
    return list_iterator_has_next(it->it);
}

dvmh_omp_interval_t *
dvmh_omp_subintervals_iterator_next(
        dvmh_omp_subintervals_iterator_t *it)
{
    assert(it != NULL);
    return (dvmh_omp_interval_t *) list_iterator_next(it->it);
}

void
dvmh_omp_subintervals_iterator_destroy(
        dvmh_omp_subintervals_iterator_t *it)
{
    assert(it != NULL);
    list_iterator_destroy(it->it);
    free(it);
}

/*
struct _dvmh_omp_interval {
    context_descriptor *descriptor;
    list *subintervals;
    double load_imbalance;
    double thread_prod_max;
    double thread_prod_min;
    double thread_prod_avg;
};
*/
