#ifndef DVMH_OMP_INTERVAL_H
#define DVMH_OMP_INTERVAL_H

#include <stdint.h>

typedef struct _dvmh_omp_interval {
    uint64_t execution_count;
    int parent_id; // TODO maybe we should remove this
    double io_time; // TODO must be aggregated. See perftool code.
    double sync_barrier_time; // TODO must be aggregated. See perftool code.
    double idle_critical_time; // TODO must be aggregated. See perftool code. Reconsider.
    double sync_flush_time; // TODO must be aggregated. See perftool code.
    double used_time; // TODO must be integrated. See perftool code.
    int used_thread_number; // TODO для каждого интервала считаем несредственно использованных. Потом интегрируем по вложенным.
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
} dvmh_omp_interval;

void
dvmh_omp_interval_add_io_time(
        dvmh_omp_interval *i,
        double io_time);

void
dvmh_omp_interval_add_barrier_time(
        dvmh_omp_interval *i,
        double barrier_time);

void
dvmh_omp_interval_add_critical_time(
        dvmh_omp_interval *i,
        double critical_time);

void
dvmh_omp_interval_add_flush_time(
        dvmh_omp_interval *i,
        double flush_time);

void
dvmh_omp_interval_add_used_time(
        dvmh_omp_interval *i,
        double used_time);

void
dvmh_omp_interval_add_exectuion_count(
        dvmh_omp_interval *i,
        uint64_t count);

#endif