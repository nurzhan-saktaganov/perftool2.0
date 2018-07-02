#include "dvmh_omp_interval.h"


void dvmh_omp_interval_set_parent_id(
        dvmh_omp_interval_t *i,
        int parent_id)
{
    i->parent_id = parent_id;
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
