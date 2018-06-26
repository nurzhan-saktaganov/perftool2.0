#include "dvmh_omp_interval.h"


void dvmh_omp_interval_set_parent_id(
        dvmh_omp_interval *i,
        int parent_id)
{
    i->parent_id = parent_id;
}

void
dvmh_omp_interval_add_exectuion_count(
        dvmh_omp_interval *i,
        uint64_t count)
{
    i->execution_count += count;
}

void
dvmh_omp_interval_add_io_time(
        dvmh_omp_interval *i,
        double io_time)
{
    i->io_time += io_time;
}

void
dvmh_omp_interval_add_barrier_time(
        dvmh_omp_interval *i,
        double barrier_time)
{
    i->sync_barrier_time += barrier_time;
}

void
dvmh_omp_interval_add_critical_time(
        dvmh_omp_interval *i,
        double critical_time)
{
    i->idle_critical_time += critical_time;
}

void
dvmh_omp_interval_add_flush_time(
        dvmh_omp_interval *i,
        double flush_time)
{
    i->sync_flush_time += flush_time;
}

void
dvmh_omp_interval_add_used_time(
        dvmh_omp_interval *i,
        double used_time)
{
    i->used_time += used_time;
}



/*
struct _dvmh_omp_interval {
    context_descriptor *descriptor;
    list *subintervals;
    events_occurrences *occurrences; //hashtable := thread_id to list of events
    int calls_count;
    double io_time;
    double execution_time;
    double sync_barrier;
    double used_time;
    int used_threads_number;
    double idle_critical;
    double sync_flush;
    double idle_parallel;
    double load_imbalance;
    double thread_prod_max;
    double thread_prod_min;
    double thread_prod_avg;
    double total_time;
    double lost_time;
    double productive_time;
    double insufficient_parallelism;
    double efficiency;
    int is_in_parallel;
};
*/