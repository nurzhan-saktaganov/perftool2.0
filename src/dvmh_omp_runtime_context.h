#ifndef DVMH_OMP_RUNTIME_CONTEXT_H
#define DVMH_OMP_RUNTIME_CONTEXT_H

#include <omp.h>

#include "context_descriptor.h"
#include "dvmh_omp_thread_context.h"

typedef struct _dvmh_omp_runtime_context_t {
    int num_threads;
    dvmh_omp_thread_context **thread_contexts;

    int num_context_descriptors;
    context_descriptor **context_descriptors;

    // lock for each interval.
    omp_lock_t *interval_locks;
    int *threads_in_interval;
    double *execution_times;
    double *end_parallel_times;
    double *idle_parallel_times;
} dvmh_omp_runtime_context_t;


dvmh_omp_runtime_context_t *
dvmh_omp_runtime_context_create(
        int num_threads,
        int num_context_descriptors);

void
dvmh_omp_runtime_context_destroy(
        dvmh_omp_runtime_context_t *ctx);

void
dvmh_omp_runtime_context_set_thread_context(
        dvmh_omp_runtime_context_t *rctx,
        dvmh_omp_thread_context *tctx,
        int thread_id);

void
dvmh_omp_runtime_context_set_context_descriptor(
        dvmh_omp_runtime_context_t *ctx,
        context_descriptor *cd,
        int cd_id);

void
dvmh_omp_runtime_context_lock_interval(
        dvmh_omp_runtime_context_t *ctx,
        int id);

void
dvmh_omp_runtime_context_unlock_interval(
        dvmh_omp_runtime_context_t *ctx,
        int id);

// must be under lock.
void
dvmh_omp_runtime_context_inc_interval_visitors(
        dvmh_omp_runtime_context_t *ctx,
        int id);

// must be under lock.
int
dvmh_omp_runtime_context_get_interval_visitors(
        dvmh_omp_runtime_context_t *ctx,
        int id);

// must be under lock.
void
dvmh_omp_runtime_context_dec_interval_visitors(
        dvmh_omp_runtime_context_t *ctx,
        int id);

// must be under lock.
void
dvmh_omp_runtime_context_add_exectuion_time(
        dvmh_omp_runtime_context_t *ctx,
        int id,
        double execution_time);

void
dvmh_omp_runtime_context_end_parallel(
    dvmh_omp_runtime_context_t *ctx,
    int thread_id,
    double when);

void
dvmh_omp_runtime_context_after_parallel(
    dvmh_omp_runtime_context_t *ctx,
    int interval_id,
    double when);

#endif
