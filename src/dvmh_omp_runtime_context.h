#ifndef DVMH_OMP_RUNTIME_CONTEXT_H
#define DVMH_OMP_RUNTIME_CONTEXT_H

#include <omp.h>

#include "context_descriptor.h"
#include "dvmh_omp_interval.h"
#include "dvmh_omp_thread_context.h"

typedef struct _dvmh_omp_runtime_context_t {
    int num_threads;
    dvmh_omp_thread_context_t *thread_contexts;

    int num_context_descriptors;
    context_descriptor **context_descriptors;

    volatile int threads_spawner_interval_id;

    // lock for each interval.
    omp_lock_t *interval_locks;
    int *threads_in_interval;
    double *execution_times; // TODO how to make it lock free?
    double *end_parallel_times;
    double *idle_parallel_times;
    bool *is_interval_in_parallel;
    bool is_parallel_region;
} dvmh_omp_runtime_context_t;

void
dvmh_omp_runtime_context_init(
        dvmh_omp_runtime_context_t *ctx,
        int num_threads,
        int num_context_descriptors);

void
dvmh_omp_runtime_context_deinit(
        dvmh_omp_runtime_context_t *ctx);

dvmh_omp_thread_context_t *
dvmh_omp_runtime_context_get_thread_context(
        dvmh_omp_runtime_context_t *ctx,
        int thread_id);

void
dvmh_omp_runtime_context_set_context_descriptor(
        dvmh_omp_runtime_context_t *ctx,
        context_descriptor *cd,
        int cd_id);

context_descriptor *
dvmh_omp_runtime_context_context_descriptor(
        dvmh_omp_runtime_context_t *ctx,
        int cd_id);

void
dvmh_omp_runtime_context_set_threads_spawner_id(
        dvmh_omp_runtime_context_t *ctx,
        int id);

void
dvmh_omp_runtime_context_unset_threads_spawner_id(
        dvmh_omp_runtime_context_t *ctx);

int
dvmh_omp_runtime_context_get_threads_spawner_id(
        dvmh_omp_runtime_context_t *ctx);

void
dvmh_omp_runtime_context_set_parallel_mode(
        dvmh_omp_runtime_context_t *ctx);

void
dvmh_omp_runtime_context_unset_parallel_mode(
        dvmh_omp_runtime_context_t *ctx);

bool
dvmh_omp_runtime_context_is_parallel_mode(
        dvmh_omp_runtime_context_t *ctx);

void
dvmh_omp_runtime_context_set_interval_non_parallel(
        dvmh_omp_runtime_context_t *ctx,
        int id);

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
dvmh_omp_runtime_context_add_execution_time(
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

dvmh_omp_interval_t *
dvmh_omp_runtime_context_integrate(
        dvmh_omp_runtime_context_t *ctx);

void
dvmh_omp_runtime_context_integrated_free(
        dvmh_omp_runtime_context_t *ctx,
        dvmh_omp_interval_t *tree);

#endif
