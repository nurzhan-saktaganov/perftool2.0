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

void dvmh_omp_runtime_context_set_context_descriptor(
        dvmh_omp_runtime_context_t *ctx,
        context_descriptor *cd,
        int cd_id);

#endif
