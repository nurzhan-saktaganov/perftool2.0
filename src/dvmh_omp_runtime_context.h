#ifndef DVMH_OMP_RUNTIME_CONTEXT_H
#define DVMH_OMP_RUNTIME_CONTEXT_H

#include "dvmh_omp_thread_context.h"

typedef struct _dvmh_omp_runtime_context_t {
    int num_threads;
    dvmh_omp_thread_context **thread_contexts;
} dvmh_omp_runtime_context_t;


dvmh_omp_runtime_context_t *
dvmh_omp_runtime_context_create(
        int num_threads);

void
dvmh_omp_runtime_context_destroy(
        dvmh_omp_runtime_context_t *ctx);

void
dvmh_omp_runtime_context_set_thread_context(
        dvmh_omp_runtime_context_t *rctx,
        dvmh_omp_thread_context *tctx,
        int thread_id);

#endif
